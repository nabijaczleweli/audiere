#!/bin/sh

die() {
  exit 1
}

VERSION=1.9.2
BASE=audiere-$VERSION-doxygen

rm -rf html $BASE         || die
doxygen audiere.doxy      || die
cp -r html $BASE          || die
zip -r $BASE.zip $BASE    || die
tar cfv $BASE.tar $BASE   || die
bzip2 -k $BASE.tar        || die
gzip $BASE.tar            || die

"C:/Program Files/HTML Help Workshop/hhc" html/index.hhp
mv html/index.chm $BASE.chm || die
