FROM python:alpine3.4
WORKDIR /app
COPY ./docker /app
RUN pip install -r ./docker/reqs.txt
ENTRYPOINT [ "python", "data.py" ]

