#!/bin/bash
# SPDX-License-Identifier: (GPL-2.0 OR MIT)
# Copyright 2018-2020 NXP

slave_position=0
axle_offset=0
usage()
{
	echo "-p | --position : specfic the slave's positon on the bus"
	echo "-f | --offset : specfic the axle's offset on the alxe"

}

while [ "$1" != "" ]; do
    case $1 in
        -p | --position )       shift
                                slave_position=$1
                                ;;
        -o | --offset )    	shift
				axle_offset=$1
                                ;;
        -h | --help )           usage
                                exit
                                ;;
        * )                     usage
                                exit 1
    esac
    shift
done

ethercat=""
ethercat=${IGH_PATH}

if [ x${ethercat} == x"" ]
then
    ethercat="ethercat"
else
    ethercat=${ethercat}"/ethercat"
fi

calc_offset() 
{

	NEW_BASE=$(( $1 + $2 * 0x800))
	printf "0x%X\n" $NEW_BASE
}

type=`${ethercat} upload -p ${slave_position} 0x1000 0x0`
type=(`echo $type`)
type=$type
echo $type
if [ ${axle_offset} -gt 0 ] 
then
	if [ $((axle_offset & 0xFFFF0000 == 0xfff))  ]
	then
		NEW_BASE=$(( 0x6fff + ${axle_offset} * 0x800))
		obj=`printf "0x%X\n" $NEW_BASE`
		type=`${ethercat} upload -p ${slave_position} ${obj} 0x0`
	fi
fi
recogninzed=0
if [ $(((${type} & 0xf1ffff) == 0x10192)) -eq 1 ]
then
	echo "Device: Frequency converter.  Device profile: ds402"
	recogninzed=1
fi

if [ $(((${type} & 0xff2ffff) == 0x20192)) -eq 1 ]
then
	echo "Device: Servo drive.  Device profile: ds402"
	recogninzed=1
fi

if [ $(((${type} & 0xf4ffff) == 0x40192)) -eq 1 ]
then
	echo "Device: Stepper motor.  Device profile: ds402"
	recogninzed=1
fi

if [ ${recogninzed} -eq 0 ] 
then
	echo "No recognized device type"
fi

NEW_BASE=$(( 0x6502 + ${axle_offset} * 0x800))
obj=`printf "0x%X\n" $NEW_BASE`
support_mode=`${ethercat} upload -p ${slave_position} ${obj} 0x0`
support_mode=(`echo $support_mode`)
support_mode=${support_mode[0]}
echo $support_mode
echo "Display the mode supported by this axle"
if [ $((${support_mode} & 0x1)) -ne 0 ]
then
	echo "	pp - Profile Position Mode",
fi

if [ $((${support_mode} & 0x2)) -ne 0 ]
then
	echo "	vl   Velocity Mode",
fi

if [ $((${support_mode} & 0x4)) -ne 0 ]
then
	echo "	pv - Profile velocity Mode",
fi

if [ $((${support_mode} & 0x8)) -ne 0 ]
then
	echo "	tq - Profile Torque Mode",
fi

if [ $((${support_mode} & 0x20)) -ne 0 ]
then
	echo "	hm - Homing Mode",
fi

if [ $((${support_mode} & 0x80)) -ne 0 ]
then
	echo "	csp - Cyclic sync Position Mode",
fi

if [ $((${support_mode} & 0x100)) -ne 0 ]
then
	echo "	csv - Cyclic sync Velocity Mode",
fi

if [ $((${support_mode} & 0x200)) -ne 0 ]
then
	echo "	cst - Cyclic sync Torque Mode"
fi
