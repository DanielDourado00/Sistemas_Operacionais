from tlb import TLB
from page_table import PageTable

class MemoryManager:
    def __init__(self, frames, algorithm):
        self.frames = frames
        self.tlb = TLB()  # Gerencia a TLB
        self.page_table = PageTable(frames, algorithm)  # Gerencia a tabela de páginas
        self.memory = [0] * (frames * 256)  # Simula a memória física (inicialmente vazia)

    def get_frame(self, page_number, stats):
        # Verificar na TLB
        frame = self.tlb.lookup(page_number)
        if frame is not None:
            stats.tlb_hit += 1
            return frame

        # Consultar tabela de páginas
        stats.tlb_miss += 1
        frame = self.page_table.lookup(page_number, self.memory, stats)

        # Atualizar TLB
        self.tlb.update(page_number, frame)
        return frame

    def read(self, physical_address):
        """
        Lê o valor armazenado na memória física no endereço físico especificado.
        """
        return self.memory[physical_address]
