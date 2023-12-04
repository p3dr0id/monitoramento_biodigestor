# Projeto de Pesquisa: Monitoramento online de um biodigestor utilizando Arduino e sensores

Este repositório possui os códigos do arduino onde estão dispostas funções para o cálculo de parâmetros da biodigestão.

As funções do código do Arduino (monitoramento_biodigestor_v2_0.ino) possuem os dados de calibração dos sensores

O código em Python (gravacao_dados.py) lê a porta serial do arduino e envia os dados para uma planilha excel e outra online do google sheets: 
(https://docs.google.com/spreadsheets/d/1pNtnnA912Z3zYJw5Aj4xiIXB5Im3NspKPM-Obb4ECGM/edit?usp=sharing)

Neste link é mostrado a configuração da API do google sheets para permitir a conexão e transmissão de dados:
(https://codoid.com/automation-testing/gspread-python-tutorial/)
