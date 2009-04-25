#!/bin/sh

for suffix in c h sh cmake txt in conf lang ebuild po pot 1 rc desktop; do
	find . -name \*.$suffix -print | xargs -i sed -i -e 's@\r@@g' {}
done
