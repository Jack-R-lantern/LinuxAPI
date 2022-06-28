#ifndef TLPI_HDR_H
#define TLPI_HDR_H				/* 실수로 두 번 포함시키는 것을 방지한다. */

#include <sys/types.h>			/* 여러 프로그램에 쓰이는 형 정의 */
#include <stdio.h>				/* 표준 I/O 함수 */
#include <stdlib.h>				/* 흔히 쓰이는 라이브러리 함수 프로토타입, EXIT_SUCCESS와 EXIT_FAILURE 상수 */
#include <unistd.h>				/* 여러 시스템 호출 프로토타입 */
#include <errno.h>				/* errno와 에러 상수 정의 */
#include <string.h>				/* 흔히 쓰이는 문자열 처리 함수 */

#include "get_num.h"			/* 수 인자 처리 함수(getInt(), getLong()) 선언 */

#include "error_functions.h"	/* 에러 처리 함수 선언 */

typedef enum { FALSE, TRUE } Boolean;

#define min(m, n) ((m) < (n) ? (m) : (n))
#define max(m, n) ((m) > (n) ? (m) : (n))

#endif