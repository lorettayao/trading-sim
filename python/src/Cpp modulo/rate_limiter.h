#ifndef RATE_LIMITER_H
#define RATE_LIMITER_H

#include <mutex>
#include <chrono>
#include <condition_variable>
#include <algorithm>
#include <iostream>

/**
 * @class RateLimiter
 * @brief Implements the Token Bucket algorithm for strictly controlling API request frequency.
 * The Token Bucket design ensures that access to the shared API resource in a multi-threaded
 * environment does not exceed the preset rate limit (e.g., 10 requests per second).
 */
class RateLimiter {
public:
    // Constructor: Sets the maximum capacity and token refill rate (tokens per second)
    RateLimiter(double capacity, double rate) 
        : tokens_(capacity), 
          capacity_(capacity), 
          rate_(rate), 
          last_refill_time_(std::chrono::steady_clock::now()) 
    {
        if (capacity <= 0 || rate <= 0) {
            std::cerr << "Error: Capacity and rate must be greater than 0." << std::endl;
        }
    }
    
    /**
     * @brief Attempts to acquire a token. If no token is available, the calling thread will be blocked until one is.
     */
    void acquire() {
        std::unique_lock<std::mutex> lock(mutex_);
        
        // Core logic loop: Continuously check for available tokens
        while (true) {
            // 1. Refill tokens
            refillTokens();
            
            if (tokens_ >= 1.0) {
                // 2. If token is available, consume one and exit the loop
                tokens_ -= 1.0;
                return;
            }
            
            // 3. If no token, calculate necessary wait time and wait on the condition variable
            double needed_time = (1.0 - tokens_) / rate_; // Calculate time (in seconds) needed to generate one token
            
            // Convert to chrono duration
            std::chrono::duration<double> wait_duration(needed_time);
            
            // Wait until a new token is available or the calculated wait time has elapsed
            cv_.wait_for(lock, std::chrono::duration_cast<std::chrono::steady_clock::duration>(wait_duration));
        }
    }

private:
    /**
     * @brief Calculates and refills tokens based on the time elapsed since the last refill.
     */
    void refillTokens() {
        auto now = std::chrono::steady_clock::now();
        // Calculate elapsed time since last refill (in seconds)
        std::chrono::duration<double> time_since_last_refill = now - last_refill_time_;
        
        // Calculate new tokens based on the rate
        double new_tokens = time_since_last_refill.count() * rate_;
        
        if (new_tokens > 0) {
            // Add new tokens to the bucket, capped by capacity
            tokens_ = std::min(capacity_, tokens_ + new_tokens);
            last_refill_time_ = now;
            
            // Notify all waiting threads that tokens are now available
            cv_.notify_all();
        }
    }

    std::mutex mutex_;                       // Mutex: Protects read/write of tokens_
    std::condition_variable cv_;            // Condition variable: Allows threads to wait for new tokens
    double tokens_;                          // Current number of tokens
    double capacity_;                        // Max capacity of the bucket
    double rate_;                            // Token refill rate (tokens/sec)
    std::chrono::steady_clock::time_point last_refill_time_; // Time of the last refill
};

#endif // RATE_LIMITER_H