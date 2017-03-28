## ngx的动态数据ngx_array_t
看一下基本结构
```
struct ngx_array_s {
    void        *elts;    // 数组元素
    ngx_uint_t   nelts;   // 当前的数据数量
    size_t       size;    // 一个元素的大小
    ngx_uint_t   nalloc;  // 数组的容量
    ngx_pool_t  *pool;    // ngx内存池
};
```
所谓数组就是一串连续的内存单元就是ngx_array_t中的elts，这是一个连续的内存单元。
主要看一下ngx_array_init以及ngx_array_push方法

### ngx_array_init
ngx通过方法ngx_array_init创建一个动态数组

### ngx_array_push
ngx通过方法ngx_array_push向动态数组中添加一个元素,代码如下
```
void *
ngx_array_push(ngx_array_t *a)
{
    void        *elt, *new;
    size_t       size;
    ngx_pool_t  *p;

    if (a->nelts == a->nalloc) {

        /* the array is full */

        size = a->size * a->nalloc;

        p = a->pool;

        if ((u_char *) a->elts + size == p->d.last
            && p->d.last + a->size <= p->d.end)
        {
            /*
             * the array allocation is the last in the pool
             * and there is space for new allocation
             */

            p->d.last += a->size;
            a->nalloc++;

        } else {
            /* allocate a new array */

            new = ngx_palloc(p, 2 * size);
            if (new == NULL) {
                return NULL;
            }

            ngx_memcpy(new, a->elts, size);
            a->elts = new;
            a->nalloc *= 2;
        }
    }

    elt = (u_char *) a->elts + a->size * a->nelts;
    a->nelts++;

    return elt;
}

```
可以看到当ngx的动态数组容量不足的时候，ngx以原数组的容量*2位新容量进行扩充(JAVA sdk也采用这个策略)。
上面的数组扩展过程有一个优化的小地方挺有意思。ngx有自己的内存池，在使用内存池的时候为了效率，ngx会故意的留一下尾部碎步。这里的优化也是针对碎片的优化。
>若果一块内存刚好分配在了一块内存池的尾部，并且尾部有足够的内存，那么动态数组扩展一个元素。

PS:ngx这种小的优化特别多，指的学习


