#pragma once

// defines for camera rotation sequences
#define CAM_ROT_SEQ_XYZ  0
#define CAM_ROT_SEQ_YXZ  1
#define CAM_ROT_SEQ_XZY  2
#define CAM_ROT_SEQ_YZX  3
#define CAM_ROT_SEQ_ZYX  4
#define CAM_ROT_SEQ_ZXY  5

#define PARSER_BUFFER_SIZE        256
#define PARSER_MAX_COMMENT 16

#define PATTERN_MAX_ARGS        16
#define PATTERN_BUFFER_SIZE      80

#define KX_OBJECT_MAX_VERTICES 1024
#define KX_OBJECT_MAX_POLYS     1024
#define RENDERLIST4DV1_MAX_POLYS          32768// 16384

#define TRANSFORM_LOCAL_ONLY       0  // perform the transformation in place on the
// local/world vertex list 
#define TRANSFORM_TRANS_ONLY       1  // perfrom the transformation in place on the 
// "transformed" vertex list

// states for objects
#define OBJECT4DV1_STATE_ACTIVE           0x0001
#define OBJECT4DV1_STATE_VISIBLE          0x0002 
#define OBJECT4DV1_STATE_CULLED           0x0004

#define TRANSFORM_LOCAL_TO_TRANS   2  // perform the transformation to the local
// vertex list, but store the results in the
// transformed vertex list

#define  KX_BEGIN namespace KevinX {
#define  KX_END };
#define USING_KX using namespace KevinX