#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definições de tamanho máximo para arrays de dados
#define MAX_FUNCIONARIOS 10
#define MAX_PACIENTES 100
#define MAX_MEDICOS 10
#define MAX_AGENDAMENTOS 200

// --- Estruturas de Dados ---
// Estrutura para representar um funcionário (para login)
typedef struct {
    char usuario[20];    // Nome de usuário para login
    char senha[20];      // Senha para login
    char profissao[20];  // Profissão do funcionário ("Medico" ou "Recepcao")
    // ID associado: se for "Medico", guarda o ID do medico correspondente;
    // se for "Recepcao", pode ser -1 ou um ID de recepção se houver uma struct Recepcao.
    int id_associado;
} Funcionario;

// Estrutura para representar um paciente
typedef struct {
    int id;              // Identificador único do paciente
    char nome[50];       // Nome completo do paciente
    char cpf[15];        // CPF do paciente
    char telefone[15];   // Telefone para contato
    char endereco[100];  // Endereço completo
} Paciente;

// Estrutura para representar um médico
typedef struct {
    int id;              // Identificador único do médico
    char nome[50];       // Nome completo do médico
    char crm[20];        // Número de registro no Conselho Regional de Medicina
    char especialidade[50]; // Especialidade do médico (ex: Ortopedia)
} Medico;

// Estrutura para representar um agendamento de consulta
typedef struct {
    int id;              // Identificador único do agendamento
    int id_paciente;     // ID do paciente agendado
    int id_medico;       // ID do médico agendado
    char data[11];       // Data da consulta (formato DD/MM/AAAA)
    char hora[6];        // Hora da consulta (formato HH:MM)
    char problema[100];  // Descrição do problema/motivo da consulta
    int realizado;       // Status da consulta (0 = não realizada, 1 = realizada)
    char diagnostico[200]; // Campo para o diagnóstico do médico
    char atestado[200];    // Campo para o atestado médico
    char laudo[200];       // Campo para o laudo médico
} Agendamento;


// --- Variáveis Globais (Arrays para armazenar os dados) ---
Funcionario funcionarios[MAX_FUNCIONARIOS];
int num_funcionarios = 0;
int proximo_id_funcionario = 0; // Para gerar IDs sequenciais

Paciente pacientes[MAX_PACIENTES];
int num_pacientes = 0;
int proximo_id_paciente = 0; // Para gerar IDs sequenciais

Medico medicos[MAX_MEDICOS];
int num_medicos = 0;
int proximo_id_medico = 0;   // Para gerar IDs sequenciais

Agendamento agendamentos[MAX_AGENDAMENTOS];
int num_agendamentos = 0;
int proximo_id_agendamento = 0; // Para gerar IDs sequenciais


// --- Protótipos das Funções ---
void limpar_buffer_entrada(); // Função auxiliar para limpar o buffer do teclado
void desenhar_linha_simples(); // Desenha uma linha simples para separação
void desenhar_linha_dupla(); // Desenha uma linha dupla para cabeçalhos
void limpar_tela(); // Limpa o terminal

// Funções de Persistência de Dados
void carregar_dados();
void salvar_dados();

// Funções de Login e Menu
int login_funcionario(char* usuario, char* senha, char* profissao_logado, int* id_logado);
void menu_medico(int id_medico_logado);
void menu_recepcao();

// Funções do Médico
void mostrar_agendamentos_medico(int id_medico_logado);
void marcar_consulta(int id_agendamento);
void aplicar_diagnostico(int id_agendamento);
void gerar_documento_medico(const char* tipo_documento, int agendamento_id, const char* texto,
                            const Paciente* paciente, const Medico* medico, const char* data_consulta, const char* hora_consulta);

// Funções da Recepção
int buscar_paciente_por_id(int id); // Auxiliar para validar paciente
int buscar_medico_por_id(int id);   // Auxiliar para validar médico
void cadastrar_paciente_recepcao();
void agendar_consulta();
void fazer_pagamento(int id_agendamento);
void emitir_nota(int id_agendamento);
void alterar_dados_cliente();
void excluir_dados_cliente();
void alterar_agendamento();
void excluir_agendamento();
void listar_consultas();


// --- Função Principal ---
int main() {
    carregar_dados(); // Carrega os dados dos arquivos ao iniciar o programa

    char usuario[20];
    char senha[20];
    char profissao_logado[20];
    int logged_in_id = -1; // Armazena o ID do usuário logado (médico ou funcionário de recepção)

    limpar_tela(); // Limpa a tela antes de mostrar o cabeçalho

    // Mensagem de Boas-Vindas
    desenhar_linha_dupla();
    printf("                  BEM-VINDO A CLINICA CRE                            \n");
    printf("          Tratamento especializado para jogadores profissionais e amadores \n");
    desenhar_linha_dupla();
    printf("\n");

    int tentativas = 0;
    while (tentativas < 3) { // Permite até 3 tentativas de login
        printf("Usuario: ");
        scanf("%19s", usuario); // %19s para evitar overflow de buffer
        limpar_buffer_entrada(); // Limpa o buffer após scanf
        printf("Senha: ");
        scanf("%19s", senha);   // %19s para evitar overflow de buffer
        limpar_buffer_entrada(); // Limpa o buffer após scanf

        // Tenta fazer o login
        int login_status = login_funcionario(usuario, senha, profissao_logado, &logged_in_id);

        if (login_status != -1) { // Login bem-sucedido
            limpar_tela();
            printf("\nLogin bem-sucedido como %s!\n", profissao_logado);
            if (strcmp(profissao_logado, "Medico") == 0) {
                menu_medico(logged_in_id); // Chama o menu do médico
            } else if (strcmp(profissao_logado, "Recepcao") == 0) {
                menu_recepcao(); // Chama o menu da recepção
            }
            break; // Sai do loop de login
        } else { // Login falhou
            printf("Usuario ou senha invalidos. Tentativas restantes: %d\n", 2 - tentativas);
            tentativas++;
        }
    }

    if (logged_in_id == -1) { // Se o limite de tentativas foi excedido
        printf("Numero maximo de tentativas excedido. Encerrando o programa.\n");
    }

    salvar_dados(); // Salva os dados nos arquivos antes de encerrar
    return 0;
}

// --- Implementações das Funções Auxiliares de Layout ---

// Limpa o buffer de entrada do teclado
void limpar_buffer_entrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Desenha uma linha simples
void desenhar_linha_simples() {
    printf("------------------------------------------------------------------------------------------------------------------\n");
}

// Desenha uma linha dupla
void desenhar_linha_dupla() {
    printf("==================================================================================================================\n");
}

// Limpa a tela do terminal (compatível com Windows e Unix/Linux/macOS)
void limpar_tela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}


// --- Implementações das Funções de Persistência ---

// Carrega os dados dos arquivos para os arrays em memória
void carregar_dados() {
    FILE *fp;
    char linha[500]; // Buffer para ler cada linha do arquivo

    printf("Carregando dados...\n");

    // Carregar Funcionários
    fp = fopen("funcionarios.txt", "r");
    if (fp != NULL) {
        while (fgets(linha, sizeof(linha), fp) != NULL && num_funcionarios < MAX_FUNCIONARIOS) {
            // Analisa a linha no formato: usuario;senha;profissao;id_associado
            sscanf(linha, "%19[^;];%19[^;];%19[^;];%d",
                   funcionarios[num_funcionarios].usuario,
                   funcionarios[num_funcionarios].senha,
                   funcionarios[num_funcionarios].profissao,
                   &funcionarios[num_funcionarios].id_associado);
            num_funcionarios++;
        }
        fclose(fp);
    }
    // Define o próximo ID de funcionário baseado nos carregados
    proximo_id_funcionario = num_funcionarios;

    // Carregar Pacientes
    fp = fopen("pacientes.txt", "r");
    if (fp != NULL) {
        while (fgets(linha, sizeof(linha), fp) != NULL && num_pacientes < MAX_PACIENTES) {
            // Analisa a linha no formato: id;nome;cpf;telefone;endereco
            sscanf(linha, "%d;%49[^;];%14[^;];%14[^;];%99[^\n]", // %99[^\n] para ler até o fim da linha
                   &pacientes[num_pacientes].id,
                   pacientes[num_pacientes].nome,
                   pacientes[num_pacientes].cpf,
                   pacientes[num_pacientes].telefone,
                   pacientes[num_pacientes].endereco);
            num_pacientes++;
        }
        fclose(fp);
    }
    proximo_id_paciente = num_pacientes;

    // Carregar Médicos
    fp = fopen("medicos.txt", "r");
    if (fp != NULL) {
        while (fgets(linha, sizeof(linha), fp) != NULL && num_medicos < MAX_MEDICOS) {
            // Analisa a linha no formato: id;nome;crm;especialidade
            sscanf(linha, "%d;%49[^;];%19[^;];%49[^\n]",
                   &medicos[num_medicos].id,
                   medicos[num_medicos].nome,
                   medicos[num_medicos].crm,
                   medicos[num_medicos].especialidade);
            num_medicos++;
        }
        fclose(fp);
    }
    proximo_id_medico = num_medicos;

    // Carregar Agendamentos
    fp = fopen("agendamentos.txt", "r");
    if (fp != NULL) {
        while (fgets(linha, sizeof(linha), fp) != NULL && num_agendamentos < MAX_AGENDAMENTOS) {
            // Analisa a linha no formato: id;id_paciente;id_medico;data;hora;problema;realizado;diagnostico;atestado;laudo
            // Usa ' ' antes de %[^\n] para consumir o newline residual e evitar problemas
            sscanf(linha, "%d;%d;%d;%10[^;];%5[^;];%99[^;];%d;%199[^;];%199[^;];%199[^\n]",
                   &agendamentos[num_agendamentos].id,
                   &agendamentos[num_agendamentos].id_paciente,
                   &agendamentos[num_agendamentos].id_medico,
                   agendamentos[num_agendamentos].data,
                   agendamentos[num_agendamentos].hora,
                   agendamentos[num_agendamentos].problema,
                   &agendamentos[num_agendamentos].realizado,
                   agendamentos[num_agendamentos].diagnostico,
                   agendamentos[num_agendamentos].atestado,
                   agendamentos[num_agendamentos].laudo);
            num_agendamentos++;
        }
        fclose(fp);
    }
    proximo_id_agendamento = num_agendamentos;

    // Se não houver funcionários, pré-popula para o primeiro uso
    if (num_funcionarios == 0) {
        printf("Dados iniciais de login e medicos criados (primeiro uso).\n");
        // Medico padrão para login
        strcpy(medicos[num_medicos].nome, "Dr. João Silva");
        strcpy(medicos[num_medicos].crm, "CRM/SP 12345");
        strcpy(medicos[num_medicos].especialidade, "Ortopedia");
        medicos[num_medicos].id = proximo_id_medico++;
        num_medicos++;

        // Funcionário medico
        strcpy(funcionarios[num_funcionarios].usuario, "medico1");
        strcpy(funcionarios[num_funcionarios].senha, "123");
        strcpy(funcionarios[num_funcionarios].profissao, "Medico");
        funcionarios[num_funcionarios].id_associado = medicos[0].id; // Associa ao primeiro médico
        num_funcionarios++;

        // Funcionário recepcao
        strcpy(funcionarios[num_funcionarios].usuario, "recepcao1");
        strcpy(funcionarios[num_funcionarios].senha, "456");
        strcpy(funcionarios[num_funcionarios].profissao, "Recepcao");
        funcionarios[num_funcionarios].id_associado = -1; // Não associado a um ID específico
        num_funcionarios++;
    }

    printf("Dados carregados com sucesso.\n");
}

// Salva os dados dos arrays em memória para os arquivos
void salvar_dados() {
    FILE *fp;

    printf("\nSalvando dados...\n");

    // Salvar Funcionários
    fp = fopen("funcionarios.txt", "w"); // "w" sobrescreve o arquivo
    if (fp != NULL) {
        for (int i = 0; i < num_funcionarios; i++) {
            fprintf(fp, "%s;%s;%s;%d\n",
                    funcionarios[i].usuario,
                    funcionarios[i].senha,
                    funcionarios[i].profissao,
                    funcionarios[i].id_associado);
        }
        fclose(fp);
    } else {
        printf("Erro ao salvar funcionarios.txt\n");
    }

    // Salvar Pacientes
    fp = fopen("pacientes.txt", "w");
    if (fp != NULL) {
        for (int i = 0; i < num_pacientes; i++) {
            fprintf(fp, "%d;%s;%s;%s;%s\n",
                    pacientes[i].id,
                    pacientes[i].nome,
                    pacientes[i].cpf,
                    pacientes[i].telefone,
                    pacientes[i].endereco);
        }
        fclose(fp);
    } else {
        printf("Erro ao salvar pacientes.txt\n");
    }

    // Salvar Médicos
    fp = fopen("medicos.txt", "w");
    if (fp != NULL) {
        for (int i = 0; i < num_medicos; i++) {
            fprintf(fp, "%d;%s;%s;%s\n",
                    medicos[i].id,
                    medicos[i].nome,
                    medicos[i].crm,
                    medicos[i].especialidade);
        }
        fclose(fp);
    } else {
        printf("Erro ao salvar medicos.txt\n");
    }

    // Salvar Agendamentos
    fp = fopen("agendamentos.txt", "w");
    if (fp != NULL) {
        for (int i = 0; i < num_agendamentos; i++) {
            // Note o uso de um caractere de substituição para novos linhas dentro de TEXT,
            // ou garantir que nao haja newline nos campos ao salvar/ler
            // Por simplicidade, assumimos que nao ha '\n' dentro dos campos de texto aqui.
            fprintf(fp, "%d;%d;%d;%s;%s;%s;%d;%s;%s;%s\n",
                    agendamentos[i].id,
                    agendamentos[i].id_paciente,
                    agendamentos[i].id_medico,
                    agendamentos[i].data,
                    agendamentos[i].hora,
                    agendamentos[i].problema,
                    agendamentos[i].realizado,
                    agendamentos[i].diagnostico,
                    agendamentos[i].atestado,
                    agendamentos[i].laudo);
        }
        fclose(fp);
    } else {
        printf("Erro ao salvar agendamentos.txt\n");
    }

    printf("Dados salvos com sucesso.\n");
}


// Função de login: verifica credenciais e retorna profissão e ID associado
int login_funcionario(char* usuario, char* senha, char* profissao_logado, int* id_logado) {
    for (int i = 0; i < num_funcionarios; i++) {
        if (strcmp(funcionarios[i].usuario, usuario) == 0 && strcmp(funcionarios[i].senha, senha) == 0) {
            strcpy(profissao_logado, funcionarios[i].profissao);
            *id_logado = funcionarios[i].id_associado;
            return 0; // Login bem-sucedido
        }
    }
    return -1; // Login falhou
}

// Menu para médicos
void menu_medico(int id_medico_logado) {
    int opcao;
    do {
        limpar_tela();
        desenhar_linha_dupla();
        printf("                   MENU DO MEDICO                                \n");
        desenhar_linha_dupla();
        printf("1. Mostrar Agendamentos\n");
        printf("2. Marcar Consulta (Realizada/Nao Realizada)\n");
        printf("3. Aplicar Diagnostico, Emitir Atestado e Laudo\n");
        printf("0. Sair\n");
        desenhar_linha_simples();
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        limpar_buffer_entrada(); // Limpa o buffer

        switch (opcao) {
            case 1:
                limpar_tela();
                mostrar_agendamentos_medico(id_medico_logado);
                break;
            case 2: {
                limpar_tela();
                int id_ag_marcar;
                printf("--- Marcar Consulta ---\n");
                printf("Informe o ID do agendamento para marcar: ");
                scanf("%d", &id_ag_marcar);
                limpar_buffer_entrada();
                marcar_consulta(id_ag_marcar);
                break;
            }
            case 3: {
                limpar_tela();
                int id_ag_diag;
                printf("--- Aplicar Diagnostico, Atestado e Laudo ---\n");
                printf("Informe o ID do agendamento: ");
                scanf("%d", &id_ag_diag);
                limpar_buffer_entrada();
                aplicar_diagnostico(id_ag_diag);
                break;
            }
            case 0:
                printf("Saindo do menu do medico.\n");
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
        }
        if (opcao != 0) {
            printf("\nPressione ENTER para continuar...");
            limpar_buffer_entrada();
        }
    } while (opcao != 0);
}

// Menu para recepção
void menu_recepcao() {
    int opcao;
    do {
        limpar_tela();
        desenhar_linha_dupla();
        printf("                   MENU DA RECEPCAO                              \n");
        desenhar_linha_dupla();
        printf("1. Cadastrar Paciente\n");
        printf("2. Agendar Consulta\n");
        printf("3. Fazer Pagamento e Emitir Nota\n");
        printf("4. Alterar Dados do Cliente\n");
        printf("5. Excluir Paciente\n");
        printf("6. Alterar Agendamento\n");
        printf("7. Excluir Agendamento\n");
        printf("8. Listar Todas as Consultas\n");
        printf("0. Sair\n");
        desenhar_linha_simples();
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        limpar_buffer_entrada(); // Limpa o buffer

        switch (opcao) {
            case 1:
                limpar_tela();
                cadastrar_paciente_recepcao();
                break;
            case 2:
                limpar_tela();
                agendar_consulta();
                break;
            case 3: {
                limpar_tela();
                int id_ag_pag;
                printf("--- Pagamento e Emissao de Nota ---\n");
                printf("Informe o ID do agendamento: ");
                scanf("%d", &id_ag_pag);
                limpar_buffer_entrada();
                fazer_pagamento(id_ag_pag);
                emitir_nota(id_ag_pag);
                break;
            }
            case 4:
                limpar_tela();
                alterar_dados_cliente();
                break;
            case 5:
                limpar_tela();
                excluir_dados_cliente();
                break;
            case 6:
                limpar_tela();
                alterar_agendamento();
                break;
            case 7:
                limpar_tela();
                excluir_agendamento();
                break;
            case 8:
                limpar_tela();
                listar_consultas();
                break;
            case 0:
                printf("Saindo do menu da recepcao.\n");
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
        }
        if (opcao != 0) {
            printf("\nPressione ENTER para continuar...");
            limpar_buffer_entrada();
        }
    } while (opcao != 0);
}

// --- Implementações das Funções do Médico ---

// Exibe os agendamentos de um médico específico
void mostrar_agendamentos_medico(int id_medico_logado) {
    desenhar_linha_dupla();
    printf("              AGENDAMENTOS DO MEDICO                              \n");
    desenhar_linha_dupla();
    int found_appointments = 0;

    printf("| %-4s | %-20s | %-10s | %-6s | %-30s | %-10s |\n",
           "ID", "Paciente", "Data", "Hora", "Problema", "Realizado");
    desenhar_linha_simples();

    for (int i = 0; i < num_agendamentos; i++) {
        if (agendamentos[i].id_medico == id_medico_logado) {
            found_appointments = 1;
            char paciente_nome[50] = "N/A";
            // Busca o nome do paciente pelo ID
            int idx_paciente = buscar_paciente_por_id(agendamentos[i].id_paciente);
            if (idx_paciente != -1) {
                strcpy(paciente_nome, pacientes[idx_paciente].nome);
            }

            printf("| %-4d | %-20.20s | %-10s | %-6s | %-30.30s | %-10s |\n",
                   agendamentos[i].id, paciente_nome, agendamentos[i].data, agendamentos[i].hora,
                   agendamentos[i].problema, (agendamentos[i].realizado ? "Sim" : "Nao"));
        }
    }
    desenhar_linha_simples();
    if (!found_appointments) {
        printf("Nenhum agendamento encontrado para este medico.\n");
    }
}

// Marca uma consulta como realizada ou não realizada
void marcar_consulta(int id_agendamento) {
    int found = 0;
    for (int i = 0; i < num_agendamentos; i++) {
        if (agendamentos[i].id == id_agendamento) {
            printf("\nAgendamento encontrado! Detalhes:\n");
            printf("  ID Agendamento: %d\n", agendamentos[i].id);
            // Verifica se o paciente existe antes de tentar acessar seu nome
            int idx_paciente = buscar_paciente_por_id(agendamentos[i].id_paciente);
            if (idx_paciente != -1) {
                printf("  Paciente: %s\n", pacientes[idx_paciente].nome);
            } else {
                printf("  Paciente: N/A (ID: %d)\n", agendamentos[i].id_paciente);
            }
            printf("  Problema: %s\n", agendamentos[i].problema);
            printf("  Status Atual: %s\n", (agendamentos[i].realizado ? "Realizada" : "Não Realizada"));
            desenhar_linha_simples();

            printf("Marcar agendamento %d como (1 para Realizada, 0 para Nao Realizada): ", id_agendamento);
            scanf("%d", &agendamentos[i].realizado);
            limpar_buffer_entrada();
            printf("Status do agendamento atualizado com sucesso.\n");
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("Agendamento com ID %d nao encontrado.\n", id_agendamento);
    }
}

// Permite ao médico aplicar diagnóstico, atestado e laudo, e gerar arquivos
void aplicar_diagnostico(int id_agendamento) {
    int found_idx = -1;
    for (int i = 0; i < num_agendamentos; i++) {
        if (agendamentos[i].id == id_agendamento) {
            found_idx = i;
            break;
        }
    }

    if (found_idx != -1) {
        Agendamento* agendamento = &agendamentos[found_idx];
        
        int idx_paciente = buscar_paciente_por_id(agendamento->id_paciente);
        Paciente* paciente = (idx_paciente != -1) ? &pacientes[idx_paciente] : NULL;

        int idx_medico = buscar_medico_por_id(agendamento->id_medico);
        Medico* medico = (idx_medico != -1) ? &medicos[idx_medico] : NULL;

        printf("\nAgendamento encontrado! Detalhes:\n");
        printf("  ID Agendamento: %d\n", agendamento->id);
        if (paciente) {
            printf("  Paciente: %s (CPF: %s)\n", paciente->nome, paciente->cpf);
        } else {
            printf("  Paciente: N/A (ID: %d)\n", agendamento->id_paciente);
        }
        if (medico) {
            printf("  Medico: %s (CRM: %s)\n", medico->nome, medico->crm);
        } else {
            printf("  Medico: N/A (ID: %d)\n", agendamento->id_medico);
        }
        printf("  Data: %s Hora: %s\n", agendamento->data, agendamento->hora);
        printf("  Problema: %s\n", agendamento->problema);
        desenhar_linha_simples();

        printf("Digite o diagnostico (max 199 chars):\n> ");
        scanf(" %199[^\n]", agendamento->diagnostico);
        limpar_buffer_entrada();

        printf("Digite o atestado (max 199 chars):\n> ");
        scanf(" %199[^\n]", agendamento->atestado);
        limpar_buffer_entrada();

        printf("Digite o laudo medico (max 199 chars):\n> ");
        scanf(" %199[^\n]", agendamento->laudo);
        limpar_buffer_entrada();

        printf("\nDiagnostico, atestado e laudo aplicados com sucesso.\n");

        char gerar_arquivos;
        printf("Deseja gerar arquivos TXT para o Atestado e Laudo Medico? (s/n): ");
        scanf(" %c", &gerar_arquivos);
        limpar_buffer_entrada();

        if (gerar_arquivos == 's' || gerar_arquivos == 'S') {
            // Gerar Atestado
            if (strlen(agendamento->atestado) > 0 && paciente && medico) { // Garante que paciente e medico não são NULL
                gerar_documento_medico("atestado", agendamento->id, agendamento->atestado,
                                       paciente, medico, agendamento->data, agendamento->hora);
            } else if (strlen(agendamento->atestado) > 0) {
                printf("Atestado vazio ou dados do paciente/medico ausentes. Nao foi gerado arquivo para atestado.\n");
            } else {
                 printf("Atestado vazio. Nao foi gerado arquivo.\n");
            }

            // Gerar Laudo
            if (strlen(agendamento->laudo) > 0 && paciente && medico) { // Garante que paciente e medico não são NULL
                gerar_documento_medico("laudo", agendamento->id, agendamento->laudo,
                                       paciente, medico, agendamento->data, agendamento->hora);
            } else if (strlen(agendamento->laudo) > 0) {
                 printf("Laudo vazio ou dados do paciente/medico ausentes. Nao foi gerado arquivo para laudo.\n");
            } else {
                printf("Laudo vazio. Nao foi gerado arquivo.\n");
            }
        }
    } else {
        printf("Agendamento com ID %d nao encontrado.\n", id_agendamento);
    }
}

// Função para gerar o arquivo TXT de atestado ou laudo
void gerar_documento_medico(const char* tipo_documento, int agendamento_id, const char* texto,
                            const Paciente* paciente, const Medico* medico, const char* data_consulta, const char* hora_consulta) {
    char nome_arquivo[100];
    char data_formatada[11]; // DD-MM-AAAA
    FILE *fp;

    // Formata a data para uso no nome do arquivo (substitui '/' por '-')
    strcpy(data_formatada, data_consulta);
    for (int i = 0; i < strlen(data_formatada); i++) {
        if (data_formatada[i] == '/') {
            data_formatada[i] = '-';
        }
    }

    // Formata o nome do arquivo
    snprintf(nome_arquivo, sizeof(nome_arquivo), "%s_agendamento_%d_%s.txt",
             tipo_documento, agendamento_id, data_formatada);

    fp = fopen(nome_arquivo, "w");
    if (fp != NULL) {
        fprintf(fp, "=========================================\n");
        fprintf(fp, "         CLINICA CRE - %s MEDICO          \n",
                (strcmp(tipo_documento, "atestado") == 0) ? "ATESTADO" : "LAUDO");
        fprintf(fp, "=========================================\n\n");

        fprintf(fp, "Referente ao Agendamento ID: %d\n", agendamento_id);
        fprintf(fp, "Data da Consulta: %s as %s\n\n", data_consulta, hora_consulta);

        if (paciente) {
            fprintf(fp, "Paciente:\n");
            fprintf(fp, "  Nome: %s\n", paciente->nome);
            fprintf(fp, "  CPF: %s\n\n", paciente->cpf);
        } else {
            fprintf(fp, "Paciente: Dados Indisponiveis\n\n");
        }

        if (medico) {
            fprintf(fp, "Medico Responsavel:\n");
            fprintf(fp, "  Nome: %s\n", medico->nome);
            fprintf(fp, "  CRM: %s\n", medico->crm);
            fprintf(fp, "  Especialidade: %s\n\n", medico->especialidade);
        } else {
            fprintf(fp, "Medico Responsavel: Dados Indisponiveis\n\n");
        }

        fprintf(fp, "-----------------------------------------\n");
        fprintf(fp, "%s:\n", (strcmp(tipo_documento, "atestado") == 0) ? "ATESTADO" : "LAUDO");
        fprintf(fp, "%s\n", texto);
        fprintf(fp, "-----------------------------------------\n\n");

        fprintf(fp, "Data de Emissao: %s (Sistema)\n", "DD/MM/AAAA - HORA"); // Implementar data/hora real se necessário
        fprintf(fp, "Assinatura do Medico: ____________________\n");
        fprintf(fp, "\n");
        fclose(fp);
        printf("Arquivo '%s' gerado com sucesso em ./%s\n", nome_arquivo, nome_arquivo);
    } else {
        printf("Erro ao criar o arquivo '%s'. Verifique permissoes ou caminho.\n", nome_arquivo);
    }
}


// --- Implementações das Funções da Recepção ---

// Função auxiliar para buscar paciente por ID
int buscar_paciente_por_id(int id) {
    for (int i = 0; i < num_pacientes; i++) {
        if (pacientes[i].id == id) {
            return i; // Retorna o índice do paciente
        }
    }
    return -1; // Paciente não encontrado
}

// Função auxiliar para buscar médico por ID
int buscar_medico_por_id(int id) {
    for (int i = 0; i < num_medicos; i++) {
        if (medicos[i].id == id) {
            return i; // Retorna o índice do médico
        }
    }
    return -1; // Médico não encontrado
}

// Cadastra um novo paciente
void cadastrar_paciente_recepcao() {
    if (num_pacientes >= MAX_PACIENTES) {
        printf("Erro: Limite de pacientes atingido (%d).\n", MAX_PACIENTES);
        return;
    }
    desenhar_linha_simples();
    printf("--- CADASTRAR NOVO PACIENTE ---\n");
    desenhar_linha_simples();
    pacientes[num_pacientes].id = proximo_id_paciente++; // Atribui ID e incrementa
    printf("Nome: ");
    scanf(" %49[^\n]", pacientes[num_pacientes].nome);
    limpar_buffer_entrada();
    printf("CPF: ");
    scanf(" %14s", pacientes[num_pacientes].cpf);
    limpar_buffer_entrada();
    printf("Telefone: ");
    scanf(" %14s", pacientes[num_pacientes].telefone);
    limpar_buffer_entrada();
    printf("Endereco: ");
    scanf(" %99[^\n]", pacientes[num_pacientes].endereco);
    limpar_buffer_entrada();

    num_pacientes++;
    printf("\nPaciente cadastrado com sucesso! ID: %d\n", pacientes[num_pacientes - 1].id);
    desenhar_linha_simples();
}

// Agenda uma nova consulta
void agendar_consulta() {
    if (num_agendamentos >= MAX_AGENDAMENTOS) {
        printf("Erro: Limite de agendamentos atingido (%d).\n", MAX_AGENDAMENTOS);
        return;
    }
    desenhar_linha_simples();
    printf("--- AGENDAR NOVA CONSULTA ---\n");
    desenhar_linha_simples();
    agendamentos[num_agendamentos].id = proximo_id_agendamento++; // Atribui ID e incrementa

    printf("ID do Paciente: ");
    scanf("%d", &agendamentos[num_agendamentos].id_paciente);
    limpar_buffer_entrada();
    // Valida se o paciente existe
    if (buscar_paciente_por_id(agendamentos[num_agendamentos].id_paciente) == -1) {
        printf("Erro: Paciente com ID %d nao encontrado. Agendamento cancelado.\n", agendamentos[num_agendamentos].id_paciente);
        proximo_id_agendamento--; // Decrementa o ID pois o agendamento não foi criado
        return;
    }

    printf("ID do Medico: ");
    scanf("%d", &agendamentos[num_agendamentos].id_medico);
    limpar_buffer_entrada();
    // Valida se o médico existe
    if (buscar_medico_por_id(agendamentos[num_agendamentos].id_medico) == -1) {
        printf("Erro: Medico com ID %d nao encontrado. Agendamento cancelado.\n", agendamentos[num_agendamentos].id_medico);
        proximo_id_agendamento--;
        return;
    }

    printf("Data (DD/MM/AAAA): ");
    scanf(" %10s", agendamentos[num_agendamentos].data);
    limpar_buffer_entrada();
    printf("Hora (HH:MM): ");
    scanf(" %5s", agendamentos[num_agendamentos].hora);
    limpar_buffer_entrada();
    printf("Problema do paciente (max 99 chars):\n> ");
    scanf(" %99[^\n]", agendamentos[num_agendamentos].problema);
    limpar_buffer_entrada();
    agendamentos[num_agendamentos].realizado = 0; // Por padrão, a consulta não foi realizada
    strcpy(agendamentos[num_agendamentos].diagnostico, ""); // Limpa campos de diagnostico
    strcpy(agendamentos[num_agendamentos].atestado, "");
    strcpy(agendamentos[num_agendamentos].laudo, "");


    num_agendamentos++;
    printf("\nConsulta agendada com sucesso! ID: %d\n", agendamentos[num_agendamentos - 1].id);
    desenhar_linha_simples();
}

// Simula o processo de pagamento
void fazer_pagamento(int id_agendamento) {
    int found = 0;
    for (int i = 0; i < num_agendamentos; i++) {
        if (agendamentos[i].id == id_agendamento) {
            printf("\nProcessando pagamento para agendamento ID %d...\n", id_agendamento);
            printf("Pagamento de R$ 150.00 realizado com sucesso para agendamento %d.\n", id_agendamento);
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("Agendamento com ID %d nao encontrado para pagamento.\n", id_agendamento);
    }
}

// Emite uma nota fiscal simplificada
void emitir_nota(int id_agendamento) {
    int found = 0;
    for (int i = 0; i < num_agendamentos; i++) {
        if (agendamentos[i].id == id_agendamento) {
            desenhar_linha_dupla();
            printf("               NOTA FISCAL SIMPLIFICADA                         \n");
            desenhar_linha_dupla();
            printf("Agendamento ID: %d\n", id_agendamento);
            
            int idx_paciente = buscar_paciente_por_id(agendamentos[i].id_paciente);
            if (idx_paciente != -1) {
                printf("Paciente: %s (CPF: %s)\n", pacientes[idx_paciente].nome, pacientes[idx_paciente].cpf);
            } else {
                printf("Paciente: N/A\n");
            }
            
            int idx_medico = buscar_medico_por_id(agendamentos[i].id_medico);
            if (idx_medico != -1) {
                printf("Medico: %s (CRM: %s)\n", medicos[idx_medico].nome, medicos[idx_medico].crm);
            } else {
                printf("Medico: N/A\n");
            }

            printf("Data da Consulta: %s as %s\n", agendamentos[i].data, agendamentos[i].hora);
            printf("Servico: Consulta Esportiva\n");
            printf("Valor: R$ 150.00\n");
            desenhar_linha_dupla();
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("Agendamento com ID %d nao encontrado para emissao de nota.\n", id_agendamento);
    }
}

// Altera os dados de um paciente
void alterar_dados_cliente() {
    desenhar_linha_simples();
    printf("--- ALTERAR DADOS DO PACIENTE ---\n");
    desenhar_linha_simples();
    int id_paciente;
    printf("Informe o ID do Paciente a ser alterado: ");
    scanf("%d", &id_paciente);
    limpar_buffer_entrada();

    int idx_paciente = buscar_paciente_por_id(id_paciente);
    if (idx_paciente != -1) {
        printf("\nPaciente encontrado! ID: %d, Nome atual: %s\n", pacientes[idx_paciente].id, pacientes[idx_paciente].nome);
        desenhar_linha_simples();
        printf("Novo Nome (atual: %s): ", pacientes[idx_paciente].nome);
        scanf(" %49[^\n]", pacientes[idx_paciente].nome);
        limpar_buffer_entrada();
        printf("Novo Telefone (atual: %s): ", pacientes[idx_paciente].telefone);
        scanf(" %14s", pacientes[idx_paciente].telefone);
        limpar_buffer_entrada();
        printf("Novo Endereco (atual: %s): ", pacientes[idx_paciente].endereco);
        scanf(" %99[^\n]", pacientes[idx_paciente].endereco);
        limpar_buffer_entrada();
        printf("\nPaciente atualizado com sucesso.\n");
    } else {
        printf("Paciente com ID %d nao encontrado.\n", id_paciente);
    }
    desenhar_linha_simples();
}

// Exclui um paciente (com confirmação)
void excluir_dados_cliente() {
    desenhar_linha_simples();
    printf("--- EXCLUIR PACIENTE ---\n");
    desenhar_linha_simples();
    int id_paciente;
    printf("Informe o ID do Paciente a ser excluido: ");
    scanf("%d", &id_paciente);
    limpar_buffer_entrada();

    int idx_paciente = buscar_paciente_por_id(id_paciente);
    if (idx_paciente != -1) {
        char confirmacao;
        printf("ATENCAO: Voce esta prestes a excluir o paciente %s (ID: %d).\n", pacientes[idx_paciente].nome, pacientes[idx_paciente].id);
        printf("Isso tambem pode invalidar agendamentos existentes. Deseja continuar? (s/n): ");
        scanf(" %c", &confirmacao);
        limpar_buffer_entrada();

        if (confirmacao == 's' || confirmacao == 'S') {
            // Lógica de exclusão: move os elementos posteriores para trás
            for (int j = idx_paciente; j < num_pacientes - 1; j++) {
                pacientes[j] = pacientes[j + 1];
            }
            num_pacientes--;
            proximo_id_paciente--; // Ajusta o próximo ID
            printf("Paciente excluido com sucesso.\n");
        } else {
            printf("Exclusao cancelada.\n");
        }
    } else {
        printf("Paciente com ID %d nao encontrado.\n", id_paciente);
    }
    desenhar_linha_simples();
}

// Altera os dados de um agendamento
void alterar_agendamento() {
    desenhar_linha_simples();
    printf("--- ALTERAR AGENDAMENTO ---\n");
    desenhar_linha_simples();
    int id_agendamento;
    printf("Informe o ID do Agendamento a ser alterado: ");
    scanf("%d", &id_agendamento);
    limpar_buffer_entrada();

    int found = 0;
    for (int i = 0; i < num_agendamentos; i++) {
        if (agendamentos[i].id == id_agendamento) {
            printf("\nAgendamento encontrado! ID: %d\n", agendamentos[i].id);
            printf("Paciente: %s\n", pacientes[buscar_paciente_por_id(agendamentos[i].id_paciente)].nome);
            printf("Medico: %s\n", medicos[buscar_medico_por_id(agendamentos[i].id_medico)].nome);
            printf("Data atual: %s, Hora atual: %s\n", agendamentos[i].data, agendamentos[i].hora);
            desenhar_linha_simples();

            printf("Nova Data (DD/MM/AAAA) (atual: %s): ", agendamentos[i].data);
            scanf(" %10s", agendamentos[i].data);
            limpar_buffer_entrada();
            printf("Nova Hora (HH:MM) (atual: %s): ", agendamentos[i].hora);
            scanf(" %5s", agendamentos[i].hora);
            limpar_buffer_entrada();
            printf("Novo Problema (atual: %s):\n> ", agendamentos[i].problema);
            scanf(" %99[^\n]", agendamentos[i].problema);
            limpar_buffer_entrada();
            printf("\nAgendamento atualizado com sucesso.\n");
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("Agendamento com ID %d nao encontrado.\n", id_agendamento);
    }
    desenhar_linha_simples();
}

// Exclui um agendamento (com confirmação)
void excluir_agendamento() {
    desenhar_linha_simples();
    printf("--- EXCLUIR AGENDAMENTO ---\n");
    desenhar_linha_simples();
    int id_agendamento;
    printf("Informe o ID do Agendamento a ser excluido: ");
    scanf("%d", &id_agendamento);
    limpar_buffer_entrada();

    int found_idx = -1;
    for (int i = 0; i < num_agendamentos; i++) {
        if (agendamentos[i].id == id_agendamento) {
            found_idx = i;
            break;
        }
    }

    if (found_idx != -1) {
        char confirmacao;
        printf("ATENCAO: Voce esta prestes a excluir o agendamento ID %d.\n", agendamentos[found_idx].id);
        printf("Deseja continuar? (s/n): ");
        scanf(" %c", &confirmacao);
        limpar_buffer_entrada();

        if (confirmacao == 's' || confirmacao == 'S') {
            // Lógica de exclusão: move os elementos posteriores para trás
            for (int j = found_idx; j < num_agendamentos - 1; j++) {
                agendamentos[j] = agendamentos[j + 1];
            }
            num_agendamentos--;
            proximo_id_agendamento--; // Ajusta o próximo ID
            printf("Agendamento excluido com sucesso.\n");
        } else {
            printf("Exclusao cancelada.\n");
        }
    } else {
        printf("Agendamento com ID %d nao encontrado.\n", id_agendamento);
    }
    desenhar_linha_simples();
}

// Lista todas as consultas agendadas
void listar_consultas() {
    desenhar_linha_dupla();
    printf("                  LISTA DE TODAS AS CONSULTAS                     \n");
    desenhar_linha_dupla();
    if (num_agendamentos == 0) {
        printf("Nenhuma consulta agendada.\n");
        desenhar_linha_simples();
        return;
    }
    printf("| %-4s | %-20s | %-20s | %-10s | %-6s | %-30s | %-10s |\n",
           "ID", "Paciente", "Medico", "Data", "Hora", "Problema", "Realizada");
    desenhar_linha_simples();
    for (int i = 0; i < num_agendamentos; i++) {
        char paciente_nome[50] = "N/A";
        int idx_paciente = buscar_paciente_por_id(agendamentos[i].id_paciente);
        if (idx_paciente != -1) {
            strcpy(paciente_nome, pacientes[idx_paciente].nome);
        }

        char medico_nome[50] = "N/A";
        int idx_medico = buscar_medico_por_id(agendamentos[i].id_medico);
        if (idx_medico != -1) {
            strcpy(medico_nome, medicos[idx_medico].nome);
        }

        printf("| %-4d | %-20.20s | %-20.20s | %-10s | %-6s | %-30.30s | %-10s |\n",
               agendamentos[i].id,
               paciente_nome,
               medico_nome,
               agendamentos[i].data,
               agendamentos[i].hora,
               agendamentos[i].problema,
               (agendamentos[i].realizado ? "Sim" : "Nao"));
    }
    desenhar_linha_simples();
}
