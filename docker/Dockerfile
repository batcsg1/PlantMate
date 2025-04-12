FROM python:alpine3.4
WORKDIR /app
COPY ./docker /app
RUN pip install --upgrade pip
RUN pip install -r /app/reqs.txt
ENTRYPOINT [ "python", "data.py" ]

