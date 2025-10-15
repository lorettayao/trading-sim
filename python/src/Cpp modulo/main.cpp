#include "rate_limiter.h"
#include <Python.h>
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <mutex>

// Global mutex to serialize output to std::cout and prevent interleaving/garbled output
std::mutex cout_mutex;

// Binance API limit is assumed to be 10 requests per second
constexpr double BINANCE_RATE_LIMIT = 10.0;
// Instantiate the Token Bucket limiter
RateLimiter limiter(BINANCE_RATE_LIMIT, BINANCE_RATE_LIMIT);

/**
 * @brief Calls the Python function to fetch data, with rate limiting applied before the call.
 * @param pFunc Pointer to the Python function object.
 * @param symbol Trading pair name.
 * @param thread_id Thread ID for logging purposes.
 */
void fetch_data_task(PyObject* pFunc, const std::string& symbol, int thread_id) {
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Thread " << thread_id << " attempting to fetch " << symbol << " data..." << std::endl;
    }

    // --- Rate Limiting Core (C++ Concurrency Control) ---
    limiter.acquire();
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Thread " << thread_id << " ✅ Successfully acquired token, starting Python API call." << std::endl;
    }
    // --- Rate Limiting Core End ---

    // ----------------------------------------------------
    // GIL Handling (Must be done before calling Python functions in a C++ thread)
    // ----------------------------------------------------
    // PyGILState_Ensure() ensures the current C++ thread owns the GIL.
    PyGILState_STATE gstate = PyGILState_Ensure();

    // Prepare Python function arguments
    PyObject* pArgs = PyTuple_New(3);
    PyTuple_SetItem(pArgs, 0, PyUnicode_FromString(symbol.c_str())); 
    PyTuple_SetItem(pArgs, 1, PyUnicode_FromString("1h"));           
    PyTuple_SetItem(pArgs, 2, PyLong_FromLong(100));                 

    PyObject* pValue = NULL;
    
    // Temporarily Release GIL for blocking I/O call
    PyThreadState *state = PyEval_SaveThread(); // Release GIL

    // Call the Python function (This is the potentially blocking network call)
    pValue = PyObject_CallObject(pFunc, pArgs);

    PyEval_RestoreThread(state); // Reacquire GIL

    // ----------------------------------------------------
    // Cleanup and Result Handling (Requires GIL)
    // ----------------------------------------------------
    Py_DECREF(pArgs); 

    if (pValue != NULL) {
        // Successful Python function call
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "Thread " << thread_id << " Data fetch complete. (Python function executed)" << std::endl;
        }
        Py_DECREF(pValue); 
    } else {
        // Handle Python exception (if pValue is NULL)
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cerr << "Thread " << thread_id << " ERROR: Python API call failed. See details below." << std::endl;
            PyErr_Print(); // Print the Python traceback
        }
    }

    // Release GIL (Matching operation for PyGILState_Ensure)
    PyGILState_Release(gstate);
}


int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(NULL);

    // ----------------------------------------------------
    // Step 1: Initialize Python Interpreter
    // ----------------------------------------------------
    Py_Initialize();
    if (!Py_IsInitialized()) {
        std::cerr << "Error: Python interpreter initialization failed." << std::endl;
        return 1;
    }
    
    // ⭐️ 核心修正: 啟用多執行緒支持
    // This call is crucial in older Python versions or some embedded environments to fully initialize GIL for multiple threads.
    // In modern Python, Py_Initialize() may call this, but explicit call ensures stability.
    PyEval_InitThreads(); 

    // Add the current directory to Python's search path so it can find fetch_data.py
    PyRun_SimpleString("import sys; sys.path.append('.')");

    // ----------------------------------------------------
    // Step 2: Import Python Module
    // ----------------------------------------------------
    PyObject* pName = PyUnicode_FromString("fetch_data"); 
    PyObject* pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule == NULL) {
        PyErr_Print();
        std::cerr << "Error: Could not import Python module (ensure fetch_data.py is in the current directory)." << std::endl;
        Py_Finalize();
        return 1;
    }

    // ----------------------------------------------------
    // Step 3: Get Python Function
    // ----------------------------------------------------
    PyObject* pFunc = PyObject_GetAttrString(pModule, "fetch_binance");

    if (pFunc == NULL || !PyCallable_Check(pFunc)) {
        if (pFunc != NULL) Py_DECREF(pFunc);
        PyErr_Print();
        std::cerr << "Error: 'fetch_binance' not found or is not a callable function." << std::endl;
        Py_DECREF(pModule);
        Py_Finalize();
        return 1;
    }

    // ----------------------------------------------------
    // Step 4: Create multiple C++ threads to competitively call the API
    // ----------------------------------------------------
    std::cout << "\n--- Starting Multi-threaded Data Fetch (Expected rate: max 10 requests per second) ---\n" << std::endl;

    std::vector<std::thread> threads;
    int num_threads = 2; // Testing with 2 threads now

    for (int i = 1; i <= num_threads; ++i) {
        threads.emplace_back(fetch_data_task, pFunc, "ETHUSDT", i);
    }

    // Wait for all threads to complete
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "\n--- All data fetching tasks complete ---\n" << std::endl;

    // ----------------------------------------------------
    // Step 5: Clean up resources
    // ----------------------------------------------------
    Py_DECREF(pFunc);
    Py_DECREF(pModule);
    Py_Finalize();

    return 0;
}
