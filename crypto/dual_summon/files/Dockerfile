FROM python:3.12

USER root
COPY server.py .
RUN pip install pycryptodome
RUN apt-get update && apt-get install socat -y
ENV flag="SECCON{dummy}"
CMD ["socat", "TCP-L:2222,fork,reuseaddr", "EXEC:'python3 server.py'"]
