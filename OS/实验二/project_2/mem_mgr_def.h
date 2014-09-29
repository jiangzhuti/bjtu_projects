#ifndef MEM_MGR_H
#define MEM_MGR_H

#define MEM_SIZE (1024 * 2)
#define TAG_FREE (char)0
#define TAG_USED (char)1

/*
 *一个内存块：
front                             back
  |                                |
  V                                V
--+----+---+----+--------+----+----+--
..|prev|tag|size|........|size|next|..
--+----+---+----+--------+----+----+--
*/

#define HEAD_SIZE (sizeof(char *) + sizeof(char) + sizeof(int))
#define FOOT_SIZE (sizeof(int) + sizeof(char *))

#define GET_PREV_FRONT(front) \
    (*(char **)(front))
#define GET_TAG_FRONT(front) \
    (*(char *)((char *)(front) + sizeof(char *)))
#define GET_SIZE_FRONT(front) \
    (*(int *)((char *)(front) + sizeof(char *) + sizeof(char)))
#define GET_NEXT_FRONT(front) \
    (*(char **)((char *)(front) + sizeof(char *) + sizeof(char) + sizeof(int) + GET_SIZE_FRONT((front)) + sizeof(int)))
#define GET_BACK_FRONT(front) \
    (void *)((char *)(front) + HEAD_SIZE + GET_SIZE_FRONT((front)) + FOOT_SIZE)
#define GET_USER_START_FRONT(front) \
    ((void *)((char *)(front) + HEAD_SIZE))
#define SET_PREV_FRONT(front, prev) \
    do { \
        *(char **)(front) = (char *)(prev); \
    } while (0)
#define SET_TAG_FRONT(front, tag) \
    do { \
        *(char *)((char *)(front) + sizeof(char *)) = (tag); \
    } while (0)
#define SET_SIZE_FRONT(front, size) \
    do { \
        *(int *)((char *)(front) + sizeof(char *) + sizeof(char)) = (size); \
        *(int *)((char *)(front) + sizeof(char *) + sizeof(char) + sizeof(int) + (size)) = (size); \
    } while (0)
#define SET_NEXT_FRONT(front, next) \
    do { \
        GET_NEXT_FRONT((front)) = (next); \
    } while (0)
#define SET_ALL_FRONT(front, size, prev, tag, next) \
    do {\
        SET_SIZE_FRONT((front), (size)); \
        SET_PREV_FRONT((front), (prev)); \
        SET_TAG_FRONT((front), (tag)); \
        SET_NEXT_FRONT((front), (next)); \
    } while (0)

#define GET_SIZE_BACK(back) \
    (*(int *)((char *)(back) - sizeof(char *) - sizeof(int)))
#define GET_TAG_BACK(back) \
    (*(char *)((char *)(back) - sizeof(char *) - sizeof(int) - GET_SIZE_BACK((back)) - sizeof(int) - sizeof(char)))
#define GET_NEXT_BACK(back) \
    (*(char **)((char *)(back) - sizeof(char *)))
#define GET_PREV_BACK(back) \
    (*(char **)((char *)(back) - sizeof(char *) - sizeof(int) - GET_SIZE_BACK((back)) - sizeof(int) - sizeof(char) - sizeof(char *)))
#define GET_FRONT_BACK(back) \
    (void *)((char *)(back) - FOOT_SIZE - GET_SIZE_BACK((back)) - HEAD_SIZE)
#define SET_PREV_BACK(back, prev) \
    do { \
        GET_PREV_BACK((back)) = (char *)(prev); \
    } while (0)
#define SET_TAG_BACK(back, tag) \
    do { \
        GET_TAG_BACK((back)) = (tag); \
    } while (0)
#define SET_SIZE_BACK(back, size) \
    do { \
        *(int *)((char *)(back) - sizeof(char *) - sizeof(int)) = (size); \
        *(int *)((char *)(back) - sizeof(char *) - sizeof(int) - (size) - sizeof(int)) = (size); \
    } while (0)
#define SET_NEXT_BACK(back, next) \
    do { \
        GET_NEXT_BACK((back)) = (next); \
    } while (0)
#define SET_ALL_BACK(back, size, prev, tag, next) \
    do {\
        SET_SIZE_BACK((back), (size)); \
        SET_PREV_BACK((back), (prev)); \
        SET_TAG_BACK((back), (tag)); \
        SET_NEXT_BACK((back), (next)); \
    } while (0)

#define LIST_FOR_EACH(head, p) \
    for (p = GET_NEXT_FRONT((head)); p != (head); p = GET_NEXT_FRONT((p)))
#define LIST_FOR_EACH_AFTER(head, p, before) \
    for (p = GET_NEXT_FRONT((before)); p != (head); p = GET_NEXT_FRONT((p)))
#define LIST_FOR_EACH_PREV(head, p) \
    for (p = GET_PREV_FRONT((head)); p != (head); p = GET_PREV_FRONT((p)))
#define LIST_FOR_EACH_PREV_BEFORE(head, p, after) \
    for (p = GET_PREV_FRONT((after)); p != (head); p = GET_PREV_FRONT((p)))
#define LIST_ADD_AFTER(p, pos) \
    do { \
        SET_NEXT_FRONT((p), GET_NEXT_FRONT(pos)); \
        SET_PREV_FRONT((p), (pos)); \
        SET_PREV_FRONT(GET_NEXT_FRONT(pos), (p)); \
        SET_NEXT_FRONT((pos), (p)); \
    } while (0)
#define LIST_ADD_BEFORE(p, pos) \
    do { \
        SET_NEXT_FRONT(p, pos); \
        SET_PREV_FRONT(p, GET_PREV_FRONT(pos)); \
        SET_NEXT_FRONT(GET_PREV_FRONT(pos), p); \
        SET_PREV_FRONT(pos, p); \
    } while (0)
#define LIST_DEL(p) \
    do { \
        SET_PREV_FRONT(GET_NEXT_FRONT(p), GET_PREV_FRONT(p)); \
        SET_NEXT_FRONT(GET_PREV_FRONT(p), GET_NEXT_FRONT(p)); \
    } while (0)

#endif // MEM_MGR_H
