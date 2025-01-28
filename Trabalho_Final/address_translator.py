class AddressTranslator:
    def __init__(self, memory_manager):
        self.memory = memory_manager

    def translate(self, logical_address, stats):
        # Extrair número da página e deslocamento
        page_number = (logical_address >> 8) & 0xFF
        offset = logical_address & 0xFF

        # Consultar a memória (TLB e tabela de páginas)
        frame_number = self.memory.get_frame(page_number, stats)

        # Construir o endereço físico
        physical_address = (frame_number << 8) | offset
        value = self.memory.read(physical_address)

        return physical_address, value
