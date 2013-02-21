#!/bin/sh

UIC="uic -tr i18n"
PAGES="path misc lookandfeel"
WIZARDS="config"
OTHERS=""

for i in ${PAGES}; do
	${UIC} -o ${i}config.h ${i}config.ui
	${UIC} -o ${i}config.cpp -impl ${i}config.h ${i}config.ui
done

for i in ${WIZARDS}; do
	n="${i}wizard"
        ${UIC} -o ${n}.h ${n}.ui
	${UIC} -o ${n}.cpp -impl ${n}.h ${n}.ui

	perl -pi -e "s/qwizard\.h/kwizard\.h/" ${n}.h
	perl -pi -e "s/QWizard/KWizard/" ${n}.{cpp,h}
done 

for i in ${OTHERS}; do
	${UIC} -o ${i}.h ${i}.ui
	${UIC} -o ${i}.cpp -impl ${i}.h ${i}.ui
done
