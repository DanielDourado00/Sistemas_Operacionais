class Statistics:
    def __init__(self):
        self.page_faults = 0
        self.tlb_hit = 0
        self.tlb_miss = 0

    def display(self):
        total_accesses = self.tlb_hit + self.tlb_miss
        print(f"Taxa de Page Faults: {self.page_faults / total_accesses:.2%}")
        print(f"Taxa de TLB Hits: {self.tlb_hit / total_accesses:.2%}")
        

#python3 main.py data/addresses.txt 128 FIFO

#para a saida ficar dentro de um txt basta redirecionar a saida com: python3 main.py data/addresses.txt 128 FIFO > saida.txt