FROM alpine:3.17

RUN apk add --no-cache gcc musl-dev

COPY . /Docker

WORKDIR /Docker

RUN gcc -o Sudoku main.c

CMD ["./Sudoku"]
