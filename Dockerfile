FROM debian:stable-slim
WORKDIR /app
COPY . /app
RUN apt-get update && apt-get install -y build-essential cmake && rm -rf /var/lib/apt/lists/*
RUN cmake . && make
CMD ["/app/quant_demo", "data/sample.csv"]
