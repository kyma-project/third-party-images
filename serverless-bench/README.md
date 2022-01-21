# Serverless-Bench

This image is based on `locustio/locust` image. It also include a locust test configuration and a simple script to run the test and output the results in json to stdout. The logs are collected via a log sink and is pushed to google big query for further analysis.  