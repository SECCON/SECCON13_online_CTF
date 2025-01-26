FROM python:3.12-alpine

RUN apk add gcc musl-dev
RUN pip install ptrlib

WORKDIR /app
ADD . /app

CMD ["python3", "/app/solve.py"]
