import yfinance as yf
import csv

def get_stock_data(ticker, start_date, end_date):
    stock = yf.Ticker(ticker)
    # data = stock.history(period="1y", interval="1h").to_dict()
    data = stock.history(start=start_date, end=end_date, interval="1h").to_dict()

    ticker = stock.info['symbol']
    timestamps = list(data['Close'].keys())
    prices = data['Close']
    volumes = data['Volume']
    open_prices = data['Open']
    high_prices = data['High']
    low_prices = data['Low']


    with open('stock_data.csv', mode='a') as file:
        writer = csv.writer(file)
        for timestamp in timestamps:
            writer.writerow([ticker, timestamp, prices[timestamp], volumes[timestamp], open_prices[timestamp], high_prices[timestamp], low_prices[timestamp]])

    print(f"Data for {ticker} has been saved to stock_data.csv")

if __name__ == "__main__":
    # Clear the file before writing the data
    with open('stock_data.csv', mode='w') as file:
        writer = csv.writer(file)
        writer.writerow(['ticker', 'timestamp', 'price', 'volume', 'open', 'high', 'low'])
    tickers = ['NVDA', 'AAPL', 'MSFT', 'AMZN', 'GOOGL', 'META', 'TSLA', 'TSM', 'AVGO', 'ORCL']
    for ticker in tickers:
        get_stock_data(ticker, '2024-01-01', '2024-11-18')

