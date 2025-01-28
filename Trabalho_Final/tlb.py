class TLB:
    def __init__(self, size=16):
        self.size = size
        self.entries = {}
        self.queue = []

    def lookup(self, page_number):
        if page_number in self.entries:
            return self.entries[page_number]
        return None

    def update(self, page_number, frame_number):
        if len(self.entries) >= self.size:
            # Substituir entrada mais antiga
            oldest = self.queue.pop(0)
            self.entries.pop(oldest)

        self.entries[page_number] = frame_number
        self.queue.append(page_number)
