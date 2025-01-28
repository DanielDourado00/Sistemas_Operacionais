def read_addresses(file_path):
    """Lê os endereços do arquivo."""
    with open(file_path, "r") as f:
        return [int(line.strip()) for line in f]

def write_output(output_file, results):
    """Escreve os resultados no arquivo de saída."""
    with open(output_file, "w") as f:
        for res in results:
            f.write(res + "\n")

def calculate_statistics(page_faults, tlb_hits, total_addresses):
    """Calcula estatísticas do simulador."""
    page_fault_rate = (page_faults / total_addresses) * 100
    tlb_hit_rate = (tlb_hits / total_addresses) * 100
    return page_fault_rate, tlb_hit_rate
