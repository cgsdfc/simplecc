const int MAX_HEAP_SIZE = 100;

int TheHeap[100];

void SwapElements(int index1, int index2) {
  int temp;
  temp = TheHeap[index1];
  TheHeap[index1] = TheHeap[index2];
  TheHeap[index2] = temp;
}

void Heapify(int index, int heap_size) {
  int largest;
  int left;
  int right;

  largest = index;
  left = 2 * index + 1;
  right = 2 * index + 2;

  if (left < heap_size) {
    if (TheHeap[left] > TheHeap[largest]) {
      largest = left;
    }
  }

  if (right < heap_size) {
    if (TheHeap[right] > TheHeap[largest]) {
      largest = right;
    }
  }

  if (largest != index) {
    SwapElements(index, largest);
    Heapify(largest, heap_size);
  }

}

void HeapSort(int heap_size) {
  int i;

  i = heap_size / 2 - 1;
  while (i >= 0) {
    Heapify(i, heap_size);
    i = i - 1;
  }

  for (i = heap_size - 1; i >= 0; i = i - 1) {
    SwapElements(0, i);
    Heapify(0, i);
  }
}

void PrintHeap(int heap_size) {
  int i;

  for (i = 0; i < heap_size; i = i + 1) {
    printf(TheHeap[i]);
  }
}

void ResetHeap {
  int i;

  for (i = 0; i < MAX_HEAP_SIZE; i = i + 1) {
    TheHeap[i] = 0;
  }
}

void main() {
  int N;
  int temp;
  int i, j;
  int array_size;

  ResetHeap;
  printf("Input # cases:");
  scanf(N);

  for (i = 0; i < N; i = i + 1) {
    printf("Input # array elements:");
    scanf(array_size);
    if (array_size > MAX_HEAP_SIZE) {
      printf("Error: array_size exceeds MAX_HEAP_SIZE");
    } else {
      printf("Input array elements:");
      for (j = 0; j < array_size; j = j + 1) {
        scanf(temp);
        TheHeap[j] = temp;
      }
      HeapSort(array_size);
      printf("Sorted array:");
      PrintHeap(array_size);
    }
  }
  printf("Bye.");
}
