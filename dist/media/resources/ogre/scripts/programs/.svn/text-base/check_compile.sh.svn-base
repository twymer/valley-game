#!/bin/sh

set -e

check_shader()
{
	name=$1
	file=$2
	high=$3
	shift 3
	low=fp20
	case $low in
		ps_2_0) low=ps_1_3;;
		ps_2_x) low=ps_2_0;;
		ps_3_0) low=ps_2_x;;
		ps_4_0) low=ps_3_0;;
	esac
	echo $name
	# Check failing for low profile
	! cgc -profile $low -oglsl -nocode $* $file 2>/dev/null 1>&2
	cgc -profile $high -oglsl -nocode $* $file > /dev/null
}

check_shader SimpleFp							LightingFp.glsl ps_2_0 -DSHADOW=0 -DNORMAL_MAP=0 -DPARALLAX_STEPS=0 -DSPECULAR=0
check_shader ShadowFp							LightingFp.glsl ps_3_0 -DSHADOW=1 -DNORMAL_MAP=0 -DPARALLAX_STEPS=0 -DSPECULAR=0
check_shader SpecularFp							LightingFp.glsl ps_2_x -DSHADOW=0 -DNORMAL_MAP=0 -DPARALLAX_STEPS=0 -DSPECULAR=1
check_shader Specular/ShadowFp					LightingFp.glsl ps_3_0 -DSHADOW=1 -DNORMAL_MAP=0 -DPARALLAX_STEPS=0 -DSPECULAR=1
check_shader OffsetMapping/SimpleFp				LightingFp.glsl ps_2_x -DSHADOW=0 -DNORMAL_MAP=1 -DPARALLAX_STEPS=3 -DSPECULAR=0
check_shader OffsetMappingFp					LightingFp.glsl ps_2_x -DSHADOW=0 -DNORMAL_MAP=1 -DPARALLAX_STEPS=7 -DSPECULAR=0
check_shader OffsetMapping/ShadowFp				LightingFp.glsl ps_4_0 -DSHADOW=1 -DNORMAL_MAP=1 -DPARALLAX_STEPS=7 -DSPECULAR=0
check_shader OffsetMapping/SpecularFp			LightingFp.glsl ps_2_x -DSHADOW=0 -DNORMAL_MAP=1 -DPARALLAX_STEPS=7 -DSPECULAR=1
check_shader OffsetMapping/Specular/ShadowFp	LightingFp.glsl ps_4_0 -DSHADOW=1 -DNORMAL_MAP=1 -DPARALLAX_STEPS=7 -DSPECULAR=1

check_shader Splatting/12	SplatFp.glsl ps_4_0 -DNUM_LAYERS=12 -DSHADOW=1 -DNUM_LIGHTS=3 -DFOG=1

