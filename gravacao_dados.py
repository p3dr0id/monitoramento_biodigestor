
import serial # para leitura e escrita de dados na porta serial
import openpyxl # para leitura e escrita de planilhas excel
import gspread # para leitura e escrita de planilhas google sheet
import keyboard # para envio de comandos de teclado e interrupção do loop infinito

porta_arduino = 'COM7'
credencial_google = 'credential.json' # arquivo com a chave de conexão da api
key_planilha_google = '1pNtnnA912Z3zYJw5Aj4xiIXB5Im3NspKPM-Obb4ECGM' # chave da planilha do gsheet
nome_aba_planilha_google = 'medidas_gsheet'
planilha_excel = 'planilha_monitoramento_excel.xlsx'
nome_aba_planilha_excel = 'medidas_excel'
saida_loop = False # variável booleana para interromper o while True (loop infinito) por comando de teclado


def teste_conexao_arduino():
    try:
        conexao = serial.Serial(porta_arduino, 9600, timeout=0.5)
        print("Conectado com sucesso a porta:", conexao.portstr)
    except serial.SerialException:
        print("Falha na conexão com a porta USB!")
        pass

def conexao_arduino():
    return serial.Serial(porta_arduino, 9600, timeout=0.5)

def teste_conexao_gsheet():
    try:
        acesso_planilha_gsheet = gspread.service_account(credencial_google)
        planilha_gsheet = acesso_planilha_gsheet.open_by_key(key_planilha_google)
        print("Conectado com a planilha google:", planilha_gsheet.title)
    except gspread.exceptions.GSpreadException:
        print("Falha na conexão com a planilha google")
        pass

def conexao_gsheet():
    acesso_planilha_gsheet = gspread.service_account(credencial_google)
    return acesso_planilha_gsheet.open_by_key(key_planilha_google)

def sai_do_loop():
    global saida_loop
    saida_loop = True

def escreve_dados_porta_serial_no_excel_e_gsheet():
    dados_arduino = conexao_arduino()
    keyboard.add_hotkey('q', lambda: sai_do_loop()) # define a tecla para sair do loop infinito
    print("Para sair do loop infinito pressione a tecla q")
    planilha_gsheet = conexao_gsheet() # conecta a planilha gsheet
    aba_planilha_gsheet = planilha_gsheet.worksheet(nome_aba_planilha_google) # seleciona a aba de gravação da planilha gsheet

    while not saida_loop:
        book = openpyxl.load_workbook(planilha_excel) # carrega uma planilha já criada (nomeie as colunas)
        aba_planilha_excel = book[nome_aba_planilha_excel] # seleciona a página para inserção de dados
        dados = str(dados_arduino.readline()) # lê os dados da porta serial
        lista = dados[2:-1].split(',') # remove caracteres "lixo" da leitura da porta e separa os dados em uma lista
        if len(lista)==6:
            aba_planilha_excel.append([lista[0], lista[1], lista[2], lista[3], lista[4], lista[5]]) # insere os dados como append na planilha excel
            book.save(planilha_excel) # grava os dados lidos na planilha excel
            book.close()
            aba_planilha_gsheet.append_row([lista[0], lista[1], lista[2], lista[3], lista[4], lista[5]]) # insere os dados como append na planilha gsheet

teste_conexao_arduino()

teste_conexao_gsheet()

escreve_dados_porta_serial_no_excel_e_gsheet()
            
print("Programa finalizado!")

