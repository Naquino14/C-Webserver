#import debian
FROM debian:latest
# copy code to container
COPY . /app
# install make
RUN apt-get update && apt-get install -y make
RUN make
# run the app
CMD ["./server"]