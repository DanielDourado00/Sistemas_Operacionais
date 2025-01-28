class ReplacementAlgorithm:
    def __init__(self, algorithm):
        self.algorithm = algorithm
        self.pages = []  # Lista que rastreia as páginas na memória

    def replace(self):
        """
        Remove uma página da memória com base no algoritmo configurado.
        FIFO: Remove a página mais antiga.
        LRU: Remove a página menos recentemente usada.
        """
        if self.algorithm == "FIFO":
            print("Substituindo com FIFO...")
            # FIFO: Remove a página mais antiga (primeira na lista)
            return self.pages.pop(0)
        elif self.algorithm == "LRU":
            print("Substituindo com LRU...")
            # LRU: Remove a página menos recentemente usada (primeira na lista)
            return self.pages.pop(0)

    def update(self, page_number):
        """
        Atualiza o estado das páginas rastreadas.
        Para LRU, move a página acessada para o final da lista.
        """
        if page_number in self.pages:
            if self.algorithm == "LRU":
                # Remove e re-insere no final para marcar como "recentemente usada"
                self.pages.remove(page_number)
        self.pages.append(page_number)  # Adiciona ao final (para FIFO ou LRU)
