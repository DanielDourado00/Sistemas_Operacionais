import sys
from address_translator import AddressTranslator
from memory import MemoryManager
from statistics import Statistics



def main():
    if len(sys.argv) != 4:
        print("Uso: python main.py <address_file> <frames> <algorithm>")
        print("<algorithm>: FIFO ou LRU")
        return

    address_file = sys.argv[1]
    frames = int(sys.argv[2])
    algorithm = sys.argv[3].upper()

    if algorithm not in {"FIFO", "LRU"}:
        print("Erro: Algoritmo de substituição inválido. Use FIFO ou LRU.")
        return

    # Inicialização dos módulos
    memory = MemoryManager(frames, algorithm)  # Passa o algoritmo correto para o MemoryManager
    translator = AddressTranslator(memory)
    stats = Statistics()

    # Processar os endereços
    with open(address_file, 'r') as file:
        for line in file:
            logical_address = int(line.strip())
            physical_address, value = translator.translate(logical_address, stats)
            print(f"Endereço Virtual: {logical_address} Endereço Físico: {physical_address} Conteúdo: {value}")

    # Exibir estatísticas
    stats.display()

if __name__ == "__main__":
    main()
    
    
"""O arquivo addresses.txt tem um padrão de endereços que não favorece distinções claras entre os algoritmos FIFO e LRU, 
especialmente na fase inicial em que ainda há muitos quadros livres.
Isso pode explicar por que as taxas de Page Faults e TLB Hits permanecem iguais."""