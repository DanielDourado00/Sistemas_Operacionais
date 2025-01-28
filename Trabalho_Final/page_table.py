from replacement import ReplacementAlgorithm

class PageTable:
    def __init__(self, frames, algorithm):
        self.frames = frames  # Número de quadros disponíveis na memória física
        self.algorithm = ReplacementAlgorithm(algorithm)  # Algoritmo de substituição (FIFO ou LRU)
        self.table = {}  # Tabela de páginas: mapeia página virtual para quadro físico
        self.free_frames = list(range(frames))  # Lista de quadros livres (inicialmente todos disponíveis)

    def lookup(self, page_number, memory, stats):
        """
        Busca o quadro físico correspondente a uma página virtual.
        Se a página não estiver na tabela, ocorre um Page Fault.
        """
        if page_number in self.table:
            # Página encontrada na tabela de páginas (sem Page Fault)
            print(f"Página {page_number} encontrada no quadro {self.table[page_number]} (Tabela de Páginas).")
            return self.table[page_number]

        # Página não encontrada -> Page Fault
        stats.page_faults += 1
        print(f"Page Fault! Página {page_number} não encontrada na tabela.")
        return self.handle_page_fault(page_number, memory)

    def handle_page_fault(self, page_number, memory):
        """
        Trata um Page Fault carregando a página correspondente do BACKING_STORE.bin
        para a memória física. Se necessário, substitui uma página existente.
        """
        if not self.free_frames:
            # Nenhum quadro livre -> Substituir uma página usando FIFO ou LRU
            old_page = self.algorithm.replace()  # Página a ser removida
            frame = self.table.pop(old_page)  # Remove a página antiga da tabela
            print(f"Substituindo página {old_page} no quadro {frame}.")
        else:
            # Há quadros livres -> Alocar o próximo quadro disponível
            frame = self.free_frames.pop(0)
            print(f"Alocando quadro livre {frame} para a página {page_number}.")

        # Ler a página correspondente do arquivo BACKING_STORE.bin
        with open('data/BACKING_STORE.bin', 'rb') as backing_store:
            # Cada página tem 256 bytes. O deslocamento é page_number * 256.
            backing_store.seek(page_number * 256)
            memory[frame * 256:(frame + 1) * 256] = list(backing_store.read(256))

        # Atualizar a tabela de páginas com a nova entrada
        self.table[page_number] = frame
        self.algorithm.update(page_number)  # Atualizar o algoritmo de substituição
        return frame

