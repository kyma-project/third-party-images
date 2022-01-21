FROM locustio/locust:2.5.1
USER root
RUN apt update -y && apt install miller curl jq -yq && apt-get clean
ADD locust.py run-benchmarks.sh /home/locust/
