import pandas as pd

from classifytrades import TradeClassification


def run_for(trades_csv, quotes_csv, test_mode=False):
    trades = pd.read_csv(
        trades_csv,
        usecols=["timestamp", "price", "size"],
    )
    trades["timestamp"] = pd.to_datetime(trades["timestamp"], format="ISO8601")
    trades["timestamp"] = (
        trades["timestamp"].dt.hour * 3600
        + trades["timestamp"].dt.minute * 60
        + trades["timestamp"].dt.second
        + trades["timestamp"].dt.nanosecond
        + trades["timestamp"].dt.microsecond / 1_000_000
    )
    trades.rename(columns={"timestamp": "time", "size": "vol"}, inplace=True)

    quotes = pd.read_csv(
        quotes_csv,
        usecols=["timestamp", "ask_price", "ask_size", "bid_price", "bid_size"],
    )
    quotes["timestamp"] = pd.to_datetime(quotes["timestamp"], format="ISO8601")
    quotes["timestamp"] = (
        quotes["timestamp"].dt.hour * 3600
        + quotes["timestamp"].dt.minute * 60
        + quotes["timestamp"].dt.second
        + quotes["timestamp"].dt.nanosecond
        + quotes["timestamp"].dt.microsecond / 1_000_000
    )
    ask = quotes
    bid = quotes.copy()

    ask.pop("bid_price")
    ask.pop("bid_size")
    ask.rename(
        columns={"timestamp": "time", "ask_price": "price", "ask_size": "vol"},
        inplace=True,
    )

    bid.pop("ask_price")
    bid.pop("ask_size")
    bid.rename(
        columns={"timestamp": "time", "bid_price": "price", "bid_size": "vol"},
        inplace=True,
    )

    c = TradeClassification(trades, ask, bid)
    c.classify("ds_3", freq=0, reduce_precision=False, bar=0.6)

    if test_mode:
        return "{} {} {} {}".format(
            len(c.df_tr["Initiator"]),
            len(c.df_tr["Step"]),
            sum(c.df_tr["Initiator"]),
            sum(c.df_tr["Step"]),
        )
    else:
        print()
        print()
        print("==== FINAL ====")
        print("trade initiator")
        print(c.df_tr["Initiator"])
        print()
        print("classification step")
        print(c.df_tr["Step"])
        print()
        print(f"ivsum={sum(c.df_tr['Initiator'])}")
        print(f"svsum={sum(c.df_tr['Step'])}")


if __name__ == "__main__":
    run_for("AAPL_trades_2021-10-26.csv", "AAPL_quotes_2021-10-26.csv")
