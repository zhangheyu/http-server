#!/usr/bin/env bash

TARGET_OUT_PUT="output"

function make_cert(){
	test -d $TARGET_OUT_PUT || mkdir -p $TARGET_OUT_PUT
	test -d $TARGET_OUT_PUT/pem || mkdir -p $TARGET_OUT_PUT/pem
	openssl genrsa -out $TARGET_OUT_PUT/pem/httpserver.key 2048
	openssl req -new -x509 -days 36500 -key $TARGET_OUT_PUT/pem/httpserver.key -out $TARGET_OUT_PUT/pem/httpservercrt.pem -subj "/C=CN/ST=Sichuan/L=Chengdu/O=vz/OU=PersonalPC
/CN=192.168.6.69"
	cat $TARGET_OUT_PUT/pem/httpserver.key $TARGET_OUT_PUT/pem/httpservercrt.pem > $TARGET_OUT_PUT/pem/httpserver.pem
	rm $TARGET_OUT_PUT/pem/httpserver.key
}




case $1 in

mkcert)
	make_cert;;
*)
        echo "Usage: $0 {mkcert}"
esac
