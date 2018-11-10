const int max_heap_size = 100;

int heap[100];

void swap(int index1, int index2) {
  int temp;
  temp = heap[index1];
  heap[index1] = heap[index2];
  heap[index2] = temp;
}

void heapify(int index, int heap_size) {
  int largest;
  int left;
  int right;

  largest = index;
  left = 2 * index + 1;
  right = 2 * index + 2;

  if (left < heap_size) {
    if (heap[left] > heap[largest]) {
      largest = left;
    }
  }

  if (right < heap_size) {
    if (heap[right] > heap[largest]) {
      largest = right;
    }
  }

  if (largest != index) {
    swap(index, largest);
    heapify(largest, heap_size);
  }

}

void heap_sort(int heap_size) {
  int i;

  i = heap_size / 2 - 1;
  while (i >= 0) {
    heapify(i, heap_size);
    i = i - 1;
  }

  for (i = heap_size - 1; i >= 0; i = i - 1) {
    swap(0, i);
    heapify(0, i);
  }
}

void print_heap(int heap_size) {
  int i;

  for (i = 0; i < heap_size; i = i + 1) {
    printf(heap[i]);
    if (i != heap_size - 1) {
      printf(" ");
    }
  }
}

void reset_heap {
  int i;

  for (i = 0; i < max_heap_size; i = i + 1) {
    heap[i] = 0;
  }
}

void main() {
  int N;
  int temp;
  int i, j;
  int array_size;

  reset_heap;
  scanf(N);

  for (i = 0; i < N; i = i + 1) {
    scanf(array_size);
    if (array_size > max_heap_size) {
      printf("array_size exceeds max_heap_size");
    } else {
      for (j = 0; j < array_size; j = j + 1) {
        scanf(temp);
        heap[j] = temp;
      }
      heap_sort(array_size);
      print_heap(array_size);
    }
  }
}