FROM python:alpine3.9
WORKDIR /app
COPY . /app
RUN pip install --upgrade pip
RUN pip install -r /app/reqs.txt
ENTRYPOINT [ "python", "data.py" ]

