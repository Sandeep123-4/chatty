FROM gcc:latest

WORKDIR /app
COPY . .

RUN g++ main.cpp -o server

CMD ["./server"]
