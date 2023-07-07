#!/bin/sh
if [ $# != 1 ]; then
    echo "USAGE: $0 databse_name"
    exit 1
fi

#database location
db=$1
sqlcmd="sqlite3 $db"

if [ -f $db ]; then
    echo "$db already existed!!"
    exit 1
else
    touch $db
fi

#-----------------------------------------------------------------------
#
#create table orderCancelData
#id		--  主键
#payId		--  支付唯一标识
#type		--	撤单类型，1回馈；2不回馈
#json		--  json数据包
#comment	--  备注
#
keySocket="CREATE TABLE orderCancelData(	\
id		INTEGER	not null primary key autoincrement, \
payId		TEXT			, \
type		INTEGER default 1   , \
json		TEXT	default ''		, \
comment		TEXT	default ''
);"

echo $keySocket | $sqlcmd

