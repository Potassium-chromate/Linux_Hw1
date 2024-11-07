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
最後再觀察一下，他是如何將其放進`begin`和`end`兩個array中。`begin[i]`儲存比`pivot`小的linked list，`begin[i+2]`儲存比`pivot`大的linked list。==最後，也是最關鍵的是`i`最後會加2，也就是說==
```c
begin[i] = left;
end[i] = list_tail(&left);
begin[i + 1] = pivot;
end[i + 1] = pivot;
begin[i + 2] = right;
end[i + 2] = list_tail(&right);

 i += 2;
```