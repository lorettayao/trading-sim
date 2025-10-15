import requests
import pandas as pd
from requests.exceptions import Timeout, ConnectionError

def fetch_binance(symbol="ETHUSDT", interval="1h", limit=100):
    url = "https://api.binance.com/api/v3/klines"
    params = {"symbol": symbol, "interval": interval, "limit": limit}
    
    try:
        # FIX: Added a 5-second timeout to prevent indefinite hanging on network issues.
        # This ensures the call returns quickly even if the network is blocked.
        response = requests.get(url, params=params, timeout=5) 
        response.raise_for_status() # Raise an exception for bad status codes (4xx or 5xx)
        data = response.json()
        
        df = pd.DataFrame(data, columns=[
            "open_time", "open", "high", "low", "close", "volume",
            "close_time", "qav", "num_trades", "taker_base_vol",
            "taker_quote_vol", "ignore"
        ])
        df["open_time"] = pd.to_datetime(df["open_time"], unit="ms")
        df["close"] = df["close"].astype(float)
        
        # Return a simple string to C++ for confirmation
        return f"Successfully fetched {symbol}"

    except (Timeout, ConnectionError) as e:
        # Print error details on the Python side
        print(f"Python Error: Network connection failed or timed out for {symbol}. Details: {e}")
        # Re-raise the exception so the C++ side catches it (pValue == NULL)
        raise
    except requests.exceptions.HTTPError as e:
        print(f"Python Error: HTTP error for {symbol}. Details: {e}")
        raise
    except Exception as e:
        print(f"Python Error: An unexpected error occurred for {symbol}. Details: {e}")
        raise

if __name__ == "__main__":
    # Example usage for standalone run
    try:
        df = fetch_binance()
        # In this context, we avoid writing to a file to keep it simple for the C++ integration
        print(f"✅ Standalone run successful.")
    except Exception as e:
        print(f"Script failed: {e}")
