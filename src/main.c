#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_COURSE_NAME 100                                   // Maximum length for the course name
#define MIN_SCORE_FULL_SCHOLARSHIP 8.0f                       // Minimum score for full scholarship
#define MIN_SCORE_PARTIAL_SCHOLARSHIP 6.0f                    // Minimum score for partial scholarship
#define MIN_SCORE_PARTIAL_NO_PUBLIC_SCHOOL_NO_DISABILITY 7.5f // Minimum score for partial scholarship under specific conditions
#define MAX_INCOME_FULL_SCHOLARSHIP 800.0f                    // Maximum per capita income for full scholarship
#define MAX_INCOME_PARTIAL_SCHOLARSHIP 1600.0f                // Maximum per capita income for partial scholarship
#define MAX_FAMILY_RECIPIENTS_FULL_SCHOLARSHIP 0              // Maximum number of family members with scholarships allowed for full scholarship
#define MAX_FAMILY_RECIPIENTS_PARTIAL_SCHOLARSHIP 1           // Maximum number of family members with scholarships allowed for partial scholarship
#define USHRT_MAX 65535                                       // Maximum value for an unsigned short integer.

typedef enum
{
  ERROR_NONE = 0,                      // No error
  ERROR_INVALID_INPUT = 1,             // Invalid input (non-numeric)
  ERROR_ZERO_OR_NEGATIVE = 2,          // Value is less than or equal to 0
  ERROR_OUT_OF_RANGE = 3,              // Value is out of the allowed range (e.g., 0 to 10)
  ERROR_BINARY_EXPECTED = 4,           // Value must be 0 or 1
  ERROR_SCHOLARSHIP_TYPE_EXPECTED = 5, // Value must be 1 (FULL) or 2 (PARTIAL)
  ERROR_COURSE_NAME_INPUT = 6,         // Error reading the course name
  ERROR_NEGATIVE_VALUE = 7,            // Negative value entered (invalid)
  ERROR_USHRT_MAX = 8,                 // Value exceeds maximum for unsigned short (65535)
} ErrorCode;

typedef enum
{
  SCHOLARSHIP_FULL = 1,
  SCHOLARSHIP_PARTIAL = 2,
} ScholarshipType;

float calculate_per_capita_income(float monthly_income, unsigned short family_size)
{
  if (monthly_income <= 0)
    return 0.0f;

  if (family_size <= 0)
    return monthly_income;

  if (family_size == 1)
    return monthly_income;

  return monthly_income / family_size;
}

float calculate_final_score(float internal_selection_score, float enem_score, float high_school_average)
{
  return (internal_selection_score * 0.4f) + (enem_score * 0.3f) + (high_school_average * 0.3f);
}

typedef struct
{
  float monthly_household_income;
  float internal_selection_score;
  float enem_score;
  float high_school_average;
  bool has_disability;
  bool is_public_school_student;
  unsigned short family_size;
  unsigned short family_scholarship_recipients;
  ScholarshipType desired_scholarship_type;
  char desired_course[MAX_COURSE_NAME];
  float per_capita_income;
  float final_score;
} Candidate;

void initialize_candidate(Candidate *candidate)
{
  if (candidate == NULL)
    return;

  candidate->monthly_household_income = 0.0f;
  candidate->internal_selection_score = 0.0f;
  candidate->enem_score = 0.0f;
  candidate->high_school_average = 0.0f;
  candidate->has_disability = false;
  candidate->is_public_school_student = false;
  candidate->family_size = 0;
  candidate->family_scholarship_recipients = 0;
  candidate->desired_scholarship_type = 0;
  memset(candidate->desired_course, 0, MAX_COURSE_NAME);
  candidate->per_capita_income = 0.0f;
  candidate->final_score = 0.0f;
}

void update_candidate_scores(Candidate *candidate)
{
  candidate->per_capita_income = calculate_per_capita_income(candidate->monthly_household_income, candidate->family_size);
  candidate->final_score = calculate_final_score(candidate->internal_selection_score, candidate->enem_score, candidate->high_school_average);
}

void clear_input_buffer()
{
  int ch;
  while ((ch = getchar()) != '\n' && ch != EOF)
    ;
  if (ch == EOF)
  {
    clearerr(stdin);
    printf("Detectado sinal de fim de arquivo (Ctrl+D). Por favor, digite novamente.\n");
  }
}

void display_error_message(ErrorCode error_code)
{
  if (error_code == ERROR_NONE)
    return;

  printf("\nResultado: Entrada INVÁLIDA!\n");

  switch (error_code)
  {
  case ERROR_INVALID_INPUT:
    printf("Motivo: Valor informado não é um número válido.\n");
    break;
  case ERROR_ZERO_OR_NEGATIVE:
    printf("Motivo: O valor informado deve ser maior que 0.\n");
    break;
  case ERROR_OUT_OF_RANGE:
    printf("Motivo: O valor informado deve estar no intervalo de 0 a 10.\n");
    break;
  case ERROR_BINARY_EXPECTED:
    printf("Motivo: O valor informado deve ser 0 (Não) ou 1 (Sim).\n");
    break;
  case ERROR_SCHOLARSHIP_TYPE_EXPECTED:
    printf("Motivo: O valor informado deve ser 1 (INTEGRAL) ou 2 (PARCIAL).\n");
    break;
  case ERROR_COURSE_NAME_INPUT:
    printf("Motivo: Erro ao ler o nome do curso.\n");
    break;
  case ERROR_NEGATIVE_VALUE:
    printf("Motivo: O valor informado não pode ser negativo.\n");
    break;
  case ERROR_USHRT_MAX:
    printf("Motivo: O valor é muito grande. O máximo permitido é 65535.\n");
    break;
  default:
    printf("Motivo: Erro desconhecido na entrada.\n");
  }

  printf("\n");
}

unsigned short read_ushort_with_min(const char *prompt, unsigned short min, ErrorCode error_code)
{
  char extra;
  long long input;
  while (true)
  {
    printf("%s", prompt);

    if (scanf("%lld%c", &input, &extra) != 2 || extra != '\n')
    {
      clear_input_buffer();
      display_error_message(ERROR_INVALID_INPUT);
      continue;
    }

    if (input > 65535)
    {
      display_error_message(ERROR_USHRT_MAX);
      continue;
    }

    if (input < min)
    {
      display_error_message(error_code);
      continue;
    }

    return (unsigned short)input;
  }
}

bool read_boolean(const char *prompt)
{
  char extra;
  int input;

  while (true)
  {
    printf("%s", prompt);

    if (scanf("%d%c", &input, &extra) != 2 || extra != '\n')
    {
      clear_input_buffer();
      display_error_message(ERROR_INVALID_INPUT);
      continue;
    }

    if (input != 0 && input != 1)
    {
      display_error_message(ERROR_BINARY_EXPECTED);
      continue;
    }

    return (bool)input;
  }
}

float read_float_in_range(const char *prompt, float min, float max, ErrorCode error_code)
{
  char extra;
  float input;

  while (true)
  {
    printf("%s", prompt);

    if (scanf("%f%c", &input, &extra) != 2 || extra != '\n')
    {
      clear_input_buffer();
      display_error_message(ERROR_INVALID_INPUT);
      continue;
    }

    if (input < min || input > max)
    {
      display_error_message(error_code);
      continue;
    }

    return input;
  }
}

void get_desired_course(Candidate *candidate)
{
  printf("Digite o nome do curso pretendido (Opcional): ");

  candidate->desired_course[0] = '\0';

  if (scanf("%99[^\n]", candidate->desired_course) != 1)
  {
    display_error_message(ERROR_COURSE_NAME_INPUT);
    candidate->desired_course[0] = '\0';
  }

  clear_input_buffer();
}

void get_desired_scholarship_type(Candidate *candidate)
{
  char extra;
  int input;
  while (true)
  {
    printf("Tipo de bolsa desejada (1 para INTEGRAL, 2 para PARCIAL): ");

    if (scanf("%d%c", &input, &extra) != 2 || extra != '\n')
    {
      clear_input_buffer();
      display_error_message(ERROR_INVALID_INPUT);
      continue;
    }

    if (input != 1 && input != 2)
    {
      display_error_message(ERROR_SCHOLARSHIP_TYPE_EXPECTED);
      continue;
    }

    candidate->desired_scholarship_type = (ScholarshipType)input;

    break;
  };
}

void get_enem_score(Candidate *candidate)
{
  candidate->enem_score = read_float_in_range(
      "Digite a Nota do ENEM (0 a 10): ",
      0.0f, 10.0f,
      ERROR_OUT_OF_RANGE);
}

void get_family_size(Candidate *candidate)
{
  candidate->family_size = read_ushort_with_min(
      "Digite o número de pessoas na família: ",
      1,
      ERROR_ZERO_OR_NEGATIVE);
}

void get_family_scholarship_recipients(Candidate *candidate)
{
  candidate->family_scholarship_recipients = read_ushort_with_min(
      "Digite o número de familiares bolsistas na universidade: ",
      0,
      ERROR_NEGATIVE_VALUE);
}

void get_has_disability(Candidate *candidate)
{
  candidate->has_disability = read_boolean("Possui deficiência? (1-Sim, 0-Não): ");
}

void get_high_school_average(Candidate *candidate)
{
  candidate->high_school_average = read_float_in_range(
      "Digite a Média Geral do Ensino Médio (0 a 10): ",
      0.0f, 10.0f,
      ERROR_OUT_OF_RANGE);
}

void get_internal_selection_score(Candidate *candidate)
{
  candidate->internal_selection_score = read_float_in_range(
      "Digite a Nota do Processo Seletivo Interno (0 a 10): ",
      0.0f, 10.0f,
      ERROR_OUT_OF_RANGE);
}

void get_is_public_school_student(Candidate *candidate)
{
  candidate->is_public_school_student = read_boolean("É aluno da rede pública? (1-Sim, 0-Não): ");
}

void get_monthly_household_income(Candidate *candidate)
{
  char extra;
  float input;
  while (true)
  {
    printf("Digite a renda mensal da família (R$): ");

    if (scanf("%f%c", &input, &extra) != 2 || extra != '\n')
    {
      clear_input_buffer();
      display_error_message(ERROR_INVALID_INPUT);
      continue;
    }

    if (input <= 0)
    {
      display_error_message(ERROR_ZERO_OR_NEGATIVE);
      continue;
    }

    candidate->monthly_household_income = input;

    break;
  }
}

void print_application_result(const Candidate *candidate, const char *rejection_reason)
{
  printf("\n===== RESULTADO DA AVALIAÇÃO DE BOLSA UNIVERSITÁRIA =====\n");
  printf("Curso pretendido: %s\n", candidate->desired_course[0] ? candidate->desired_course : "Não informado");
  printf("Nota Final: %.2f\n", candidate->final_score);
  printf("Renda per capita: R$ %.2f\n", candidate->per_capita_income);

  if (rejection_reason)
  {
    printf("Resultado: Candidatura INDEFERIDA!\n");
    printf("Motivo(s):\n%s", rejection_reason);
  }
  else
  {
    printf("Resultado: Bolsa %s CONCEDIDA!\n",
           candidate->desired_scholarship_type == SCHOLARSHIP_FULL ? "Integral" : "Parcial");
  }

  printf("=========================================================\n");
}

int main()
{
  Candidate candidate;
  initialize_candidate(&candidate);

  printf("===== AVALIAÇÃO DE BOLSA UNIVERSITÁRIA =====\n");
  get_monthly_household_income(&candidate);
  get_internal_selection_score(&candidate);
  get_enem_score(&candidate);
  get_high_school_average(&candidate);
  get_has_disability(&candidate);
  get_is_public_school_student(&candidate);
  get_family_size(&candidate);
  get_family_scholarship_recipients(&candidate);
  get_desired_scholarship_type(&candidate);
  get_desired_course(&candidate);

  update_candidate_scores(&candidate);

  char all_rejection_reasons[1024] = {0};
  int reason_count = 0;

  switch (candidate.desired_scholarship_type)
  {
  case SCHOLARSHIP_FULL:
    if (candidate.per_capita_income > MAX_INCOME_FULL_SCHOLARSHIP)
    {
      char temp[256];
      sprintf(temp, "- Renda per capita superior a R$ %.2f. \n", MAX_INCOME_FULL_SCHOLARSHIP);
      strcat(all_rejection_reasons, temp);
      reason_count++;
    }

    if (candidate.final_score < MIN_SCORE_FULL_SCHOLARSHIP)
    {
      char temp[256];
      sprintf(temp, "- Nota final inferior a %.1f. \n", MIN_SCORE_FULL_SCHOLARSHIP);
      strcat(all_rejection_reasons, temp);
      reason_count++;
    }

    if (!candidate.is_public_school_student && !candidate.has_disability)
    {
      strcat(all_rejection_reasons, "- Não possui deficiência e não é da rede pública. \n");
      reason_count++;
    }

    if (candidate.family_scholarship_recipients > MAX_FAMILY_RECIPIENTS_FULL_SCHOLARSHIP)
    {
      char temp[256];

      if (MAX_FAMILY_RECIPIENTS_FULL_SCHOLARSHIP == 0)
      {
        sprintf(temp, "- Já possui familiar bolsista. \n");
      }
      else
      {
        sprintf(temp, "- Número de familiares bolsistas superior ao permitido (máximo de %d familiar%s). \n",
                MAX_FAMILY_RECIPIENTS_FULL_SCHOLARSHIP,
                (MAX_FAMILY_RECIPIENTS_FULL_SCHOLARSHIP != 1) ? "es" : "");
      }

      strcat(all_rejection_reasons, temp);
      reason_count++;
    }

    break;

  case SCHOLARSHIP_PARTIAL:
    if (candidate.per_capita_income > MAX_INCOME_PARTIAL_SCHOLARSHIP)
    {
      char temp[256];
      sprintf(temp, "- Renda per capita superior a R$ %.2f. \n", MAX_INCOME_PARTIAL_SCHOLARSHIP);
      strcat(all_rejection_reasons, temp);
      reason_count++;
    }

    float min_score = MIN_SCORE_PARTIAL_SCHOLARSHIP;

    if (!candidate.is_public_school_student && !candidate.has_disability)
    {
      min_score = MIN_SCORE_PARTIAL_NO_PUBLIC_SCHOOL_NO_DISABILITY;
    }

    if (candidate.final_score < min_score)
    {
      char score_message[256];

      if (min_score == MIN_SCORE_PARTIAL_NO_PUBLIC_SCHOOL_NO_DISABILITY)
      {
        sprintf(score_message, "- Nota final inferior a %.1f, exigido para a Bolsa Parcial. Candidato não é da rede pública e não possui deficiência. \n", min_score);
      }
      else
      {
        sprintf(score_message, "- Nota final inferior a %.1f. \n", min_score);
      }

      strcat(all_rejection_reasons, score_message);
      reason_count++;
    }

    if (candidate.family_scholarship_recipients > MAX_FAMILY_RECIPIENTS_PARTIAL_SCHOLARSHIP)
    {
      char temp[256];
      sprintf(temp, "- Número de familiares bolsistas superior ao permitido (máximo de %d familiar%s). \n",
              MAX_FAMILY_RECIPIENTS_PARTIAL_SCHOLARSHIP,
              (MAX_FAMILY_RECIPIENTS_PARTIAL_SCHOLARSHIP != 1) ? "es" : "");
      strcat(all_rejection_reasons, temp);
      reason_count++;
    }

    break;
  }

  print_application_result(&candidate, reason_count > 0 ? all_rejection_reasons : NULL);
  return 0;
}
