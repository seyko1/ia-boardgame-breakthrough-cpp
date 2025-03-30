#!/bin/bash
LUDII_JAR_FILE="Ludii-1.3.14.jar"

rm -f FgPlayer.jar ;
if [ ! -f ${LUDII_JAR_FILE} ];then
  echo "Ludii jar file is missing" ;
  exit 0 ;
fi
javac -cp ${LUDII_JAR_FILE} FgPlayer.java ;
if [ ! -d "bk" ];then
  mkdir bk ;
fi
mv FgPlayer.class bk/ ;
jar cf FgPlayer.jar bk/FgPlayer.class ;
