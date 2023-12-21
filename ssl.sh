#!/bin/bash

openssl genrsa -out ca.key
openssl req -new -key ca.key -out ca.csr -subj /name=ca
openssl x509 -req -in ca.csr -key ca.key -out ca.crt

openssl genrsa -out server.key
openssl req -new -key server.key -out server.csr -subj /name=server
openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -out server.crt
