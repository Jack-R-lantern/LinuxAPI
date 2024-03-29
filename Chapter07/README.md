# 07 메모리 할당
## 07.01 힙에 메모리 할당하기
> 프로세스는 힙의 크기를 증가시킴으로써 메모리를 할당할 수 있음.\
> 힙은 프로세스의 초기화되지 않은 데이터 세그먼트 바로 뒤에서 시작하는 가변 크기 세그먼트로, 연속된 가상 메모리로 이뤄져 있음.\
> 힙의 현재 한도를 `프로그램 브레이크(program break)`라고 함.\
> C 프로그램은 보통 malloc 계열의 함수를 사용, 그보다 먼저 malloc의 기반을 이루는 `brk()`, `sbrk()`를 알아야함.
### 07.01.01 프로그램 브레이크 조정하기: brk(), sbrk()
> 힙의 크기를 바꾸는 방법은 사실 간단해서, 커널에게 프로세스의 프로그램 브레이크를 조정하라고 하면 됨.\
> 처음 프로그램 브레이크는 초기하되지 않은 데이터 세그먼트의 끝 바로 뒤에 있음.\
> 프로그램 브레이크가 증가되고 나면 프로그램은 새로 할당된 영역 내의 어느 주소든 접근할 수 있음.\
> 그러나 아직 물리적 메모리 페이지가 할당된 것은 아님.\
> 커널은 프로세스가 이 페이지에 처음 접근하려고 할 때 자동으로 새로운 물리적 페이지를 할당.\
> 전통적으로 유닉스 시스템은 프로그램 브레이크를 다루는 두 가지 시스템 호출 `brk()`, `sbrk()`을 제공.\
> 위 두 함수들은 모두 리눅스에도 존재함.\
> 두 함수를 직접 사용하는 경우는 거의 없지만, 이들을 이해하면 메모리 할당이 어떻게 동작하는지를 이해하는 데 도움이 됨.
>```c
>#include <unistd.h>
>
>int brk(void *end_data_segment);
>		성공하면 0을 리턴하고, 에러가 발생하면 -1을 리턴한다.
>void *sbrk(intptr_t increment);
>		성공하면 0을 리턴하고, 에러가 발생하면 (void*) -1을 리턴한다.
>```
> `brk()`는 프로그램 브레이크를 `end_data_segment`가 가리키는 위치로 설정.\
> 가상 메모리는 페이지 단위로 할당되므로, `end_data_segment`는 실제로는 다음 페이지 경계로 올림.\
> 프로그램 브레이크를 초기값 아래로 설정하면, 어떤 일이 일어날지 알 수 없음.\
> 초기화되거나 초기화되지 않은 데이터 세그먼트의 현재 존재하지 않는 부분에 접근하려고 할 때 세그먼테이션 폴드가 발생할 수도 있음.\
> 프로그램 브레이크를 설정할 수 있는 정확한 상한은 데이터 세그먼트 크기에 대한 프로세스 자원 한도, 그리고 메모리 매핑, 공유 메모리 세그먼트, 공유 라이브러리의 위치 등 여러 가지 요인에 따라 달라짐.\
> `sbrk()`는 프로그램 브레이크를 `increment`만큼 증가시킴.\
> intptr_t형은 increment가 정수 데이터형임을 선언.\
> 성공하면 `sbrk()`는 프로그램 브레이크의 이전 주소를 리턴.\
> 즉 프로그램 브레이크를 증가시키면 리턴값은 새로 할당된 메모리 블록의 시작 주소.

### 07.01.02 힙에 메모리 할당하기: malloc()과 free()
> #### malloc
>> 일반적으로 C 프로그램은 `malloc` 계열 함수를 써서 힙에 메모리를 할당하고 해제.
>> `malloc()` 함수는 `brk()`, `sbrk()`에 비해 특히 다음과 같은 장점이 있음.
>> * C언어의 일부로 표준화되어 있음.
>> * 멀티스레드 프로그램에서 쓰기 쉽다.
>> * 작은 단위로 메모리를 할당하는 간단한 인터페이스를 제공.
>> * 임의로 메모리 블록을 해제할 수 있음. 해제된 메모리 블록은 프리 리스트로 관리되어 추후 메모리 할당 시 재활용 됨.
>> `malloc()` 함수는 힙에서 size 바이트를 할당하고 새로 할당된 메모리 블록의 시작을 가리키는 포인터를 리턴.\
>> 할당된 블록은 초기화되어 있지 않음.
>>```c
>>#include <stdlib.h>
>>
>> void *malloc(size_t size);
>>		성공하면 할당된 메모리를 가리키는 포인터를 리턴하고, 에러가 발생하면 NULL을 리턴한다.
>>```
>> `malloc`이 리턴하는 메모리 블록은 어떤 형의 C 데이터 구조도 효율적으로 접근 할 수 있도록 언제나 적절한 바이트 경계에 정렬되어 있음.\
>> 이는 대부분의 아키텍처에서 8바이트나 16바이트 경계에 맞춰 할당됨을 뜻함.\
>> 메모리를 할당하지 못하면, `malloc()`은 NULL을 리턴하고 `errno`를 설정해 에러를 알림.\
>> 메모리 할당 실패 가능성은 낮지만, 모든 `malloc()`과 관련된 함수 호출은 이런 에러 리턴을 확인해야 함.
> #### free
>>```c
>>#include <stdlib.h>
>>
>>void free(void *ptr);
>>```
>> `free()` 함수는 ptr이 가리키는 메모리 블록을 해제함.\
>> ptr은 이전에 `malloc()`이나 나중에 설명할 기타 힙 메모리 할당 함수가 리턴한 주소여야 함.|
>> 일반적으로 `free()`는 프로그램 브레이크를 낮추지 않고, 해당 메모리 블록을 이후의 `malloc()` 호출 때 재활용할 프리 블록 리스트에 추가함.\
>> 추가하는 이유는 다음과 같음.
>> * 해제된 메모리 블록은 힙의 끝보다는 보통 중간 어딘가에 위치하므로, 프로그램 브레이크를 낮출 수 없음.
>> * 프로그램이 수행할 `sbrk()` 호출 횟수를 줄일 수 있음.
>> * 많은 경우, 메모리를 많이 할당하는 프로그램에게는 프로그램 브레이크를 낮추는것이 도움이 되지 않음. 그런 프로그램은 할당된 메모리를 모두 해제하고 오랫동안 실행하기보다는, 보통 할당된 메모리를 계속 유지하거나 해제하고 다시 할당하기를 반복하기 때문.
>> `free()`의 인자가 NULL 포인터면, 해당 호출은 아무 일도 하지 않음.\
>> `free()` 호출 이후에 ptr을 사용하면, 예를 들어 ptr을 한 번 더 `free()`에 넘기면 예측할 수 없는 에러를 야기.