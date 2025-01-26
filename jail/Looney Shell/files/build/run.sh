#!/bin/sh

cd $(dirname $0)/work
exec bash --rcfile .bashrc -ri
