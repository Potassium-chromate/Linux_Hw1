# Linux_Hw1

## 測驗一
### 程式碼解釋

quick sort會用到三個linked list
- `left`: 小於`pivot->value`的linked list
- `pivot`: pivot本身
- `right`: 大於`pivot->value`的linked list

首先在while迴圈裡面，我們可以發現，他其實就在做上面提到的事。將大於`pivot->value`的節點接到`right`。反之，將小於`pivot->value`的節點接到`left`。
```c
 while (p) {
    node_t *n = p;
    p = p->next;
    list_add(n->value > value ? &right : &left, n);
}
```
最後再觀察一下，他是如何將其放進`begin`和`end`兩個array中。`begin[i]`儲存比`pivot`小的linked list，`begin[i+2]`儲存比`pivot`大的linked list。==最後，也是最關鍵的是`i`最後會加2，也就是說在下一個iteration會從上次的`right`開始，直到最後的right只剩一個節點。==
```c
begin[i] = left;
end[i] = list_tail(&left);
begin[i + 1] = pivot;
end[i + 1] = pivot;
begin[i + 2] = right;
end[i + 2] = list_tail(&right);

 i += 2;
```
在將排序好的連結串列插入最終回傳的串列時，可以觀察整體迴圈的結構。判斷式 `L != R` 代表當左右兩邊的linked list頭尾不相等時，表示該linked list不只包含一個節點；而在 `else` 內，則代表該linked list只剩一個節點，此時可以直接將該節點插入最終的要回傳linked list。

然後，還可以發現在`else`中會將`i`減1，再搭配上面提到的部分。可以發現當`right`只剩一個節點並執行`i--`後，`begin[i]`就會退回pivot，然而pivot必定只有一個節點，因此又會再做一次`i--`，然後`begin[i]`又會退回`left`的部分，最後便可以持續分割原本的`left`並對其做排序。
```c
 while (i >= 0) {
    node_t *L = begin[i], *R = end[i];
    if (L != R) {
        ...
        i += 2;
    } else {
        if (L)
            list_add(&result, L);
        i--;
    }
}
```

最後，我們可以用Graphviz來觀察一下quick_sort中`right`, `left`與`pivot`的變化。
這是最一開始的狀態。共10個節點。
![image](https://hackmd.io/_uploads/S17br7cZke.png)
一開始一定是取第一個節點作為pivot，也就是`8`。因此我們可以發現`right`, `left`與`pivot`會變成以下狀態。
![image](https://hackmd.io/_uploads/r1MKLm5b1l.png)
之後，`9`和`8`就會被併入最終的要回傳linked list。
之後再對原本的那條left做排序即可。可以發現，==整結構其實就是一個stack==。

### 改進方法
#### stack memory allocation
對於非常大的陣列，動態分配記憶體空間會比靜態分配安全。
```diff
- node_t *begin[max_level], *end[max_level];
+ node_t **begin = malloc(max_level * sizeof(node_t*));
+ node_t **end = malloc(max_level * sizeof(node_t*));
+ free(begin);
+ free(end);
```

#### deepest depth observation
首先，先來觀察一下在執行過程中`begin`和`end`會達到的最大深度。可以透過在`quick_sort`中加入以下程式碼來達成。
```c
while(){
    ...
    deepest = (deepest > i)? deepest: i;
}
printf("size : %d, deepest level : %d\n", n, deepest);
```
結果如下:

| size | deepest level |
| -------- | -------- |
| 10     | 8     |
| 100     | 16    |
| 1000     | 26    |
| 10000     | 46    |
| 100000     | 70    |
| 1000000     | 138    |
| 10000000     | 656    |

![image](https://hackmd.io/_uploads/BkTo-EoWJx.png)
可以發現實際上根本不用到分配到`2*n`那麼多記憶體。實際上$10 \sqrt{n}$便已足夠。

#### ramdom pivot
在原本的`quick_sort`中，`pivot`是取linked list的第一個節點。因此可以發現，當待排序的linked list是由decend order排序時，會產生worst case。同時，depest depth也會來到驚人的n。為了解決此問題，我們可以隨機選擇pivot來避免worst case的發生。
function如下:
```c
void random_pivot(node_t **head) {
    if (!head || !(*head)) {
        perror("Input list is NULL!\n");
        return;
    }

    int length = list_length(head);
    if (length <= 1) {
        return;
    }

    // Generate a random index within the list's length
    int idx = rand() % length;
    if (idx == 0) {
        return;
    }

    // Traverse to the node just before the pivot node
    node_t *prev = *head;
    for (int i = 1; i < idx; i++) {
        prev = prev->next;
    }

    
    node_t *pivot = prev->next;
    prev->next = pivot->next; 
    pivot->next = *head; 
    *head = pivot;  
}
```

結果如下:

| size | deepest level |
| -------- | -------- |
| 10     | 4     |
| 100     | 14    |
| 1000     | 28    |
| 10000     | 38    |
| 100000     | 54    |
| 1000000     | 94    |
| 10000000     | 378   |

可以發現，效能確實得到改善。

#### 利用 Linux Kernel API 改寫
