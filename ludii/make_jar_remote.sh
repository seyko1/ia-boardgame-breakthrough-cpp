#!/bin/bash
LUDII_JAR_FILE="Ludii-1.3.14.jar"

rm -f FgPlayerRem.jar ;
if [ ! -f ${LUDII_JAR_FILE} ];then
  echo "Ludii jar file is missing" ;
  exit 0 ;
fi
javac -cp ${LUDII_JAR_FILE} FgPlayerRem.java ;
if [ ! -d "bk" ];then
  mkdir bk ;
fi
mv FgPlayerRem.class bk/ ;
jar cf FgPlayerRem.jar bk/FgPlayerRem.class ;
