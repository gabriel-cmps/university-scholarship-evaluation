#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef enum
{
  ERROR_INVALID_INPUT = 1,             // Invalid input (non-numeric)
  ERROR_ZERO_OR_NEGATIVE = 2,          // Value is less than or equal to 0
  ERROR_OUT_OF_RANGE = 3,              // Value is out of the allowed range (e.g., 0 to 10)
  ERROR_BINARY_EXPECTED = 4,           // Value must be 0 or 1
  ERROR_SCHOLARSHIP_TYPE_EXPECTED = 5, // Value must be 1 (FULL) or 2 (PARTIAL)
  ERROR_COURSE_NAME_INPUT = 6,         // Error reading the course name
  ERROR_NEGATIVE_VALUE = 7,            // Negative value entered (invalid)
} ErrorCode;

#define MAX_COURSE_NAME 100                                   // Maximum length for the course name
#define MIN_SCORE_FULL_SCHOLARSHIP 8.0f                       // Minimum score for full scholarship
#define MIN_SCORE_PARTIAL_SCHOLARSHIP 6.0f                    // Minimum score for partial scholarship
#define MIN_SCORE_PARTIAL_NO_PUBLIC_SCHOOL_NO_DISABILITY 7.5f // Minimum score for partial scholarship under specific conditions
#define MAX_INCOME_FULL_SCHOLARSHIP 800.0f                    // Maximum per capita income for full scholarship
#define MAX_INCOME_PARTIAL_SCHOLARSHIP 1600.0f                // Maximum per capita income for partial scholarship

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
  while (getchar() != '\n')
    ;
}

void display_error_message(ErrorCode error_code)
{
  printf("\nResultado: Entrada INVÁLIDA!\n");

  switch (error_code)
  {
  case ERROR_INVALID_INPUT:
    printf("Motivo: Valor informado não é um número inteiro positivo válido.\n");
    break;
  case ERROR_ZERO_OR_NEGATIVE:
    printf("Motivo: Valor informado é menor ou igual a 0.\n");
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
  default:
    printf("Motivo: Erro desconhecido na entrada.\n");
  }
  printf("\n");
}

void get_desired_course(Candidate *candidate)
{
  printf("Digite o nome do curso pretendido (Opcional): ");

  candidate->desired_course[0] = '\0';

  clear_input_buffer();

  scanf("%99[^\n]", candidate->desired_course);

  clear_input_buffer();
}

int get_desired_scholarship_type(Candidate *candidate)
{
  while (1)
  {
    printf("Tipo de bolsa desejada (1 para INTEGRAL, 2 para PARCIAL): ");

    if (scanf("%d", (int *)&candidate->desired_scholarship_type) != 1)
    {
      display_error_message(ERROR_INVALID_INPUT);
      clear_input_buffer();
      candidate->desired_scholarship_type = 0;
      continue;
    }

    if (candidate->desired_scholarship_type != 1 && candidate->desired_scholarship_type != 2)
    {
      display_error_message(ERROR_SCHOLARSHIP_TYPE_EXPECTED);
      continue;
    }

    break;
  };
}

int get_enem_score(Candidate *candidate)
{
  while (1)
  {
    printf("Digite a Nota do ENEM (0 a 10): ");

    if (scanf("%f", &candidate->enem_score) != 1)
    {
      display_error_message(ERROR_INVALID_INPUT);
      clear_input_buffer();
      candidate->enem_score = 0.0f;
      continue;
    }

    if (candidate->enem_score < 0 || candidate->enem_score > 10)
    {
      display_error_message(ERROR_OUT_OF_RANGE);
      continue;
    }

    break;
  }
}

int get_family_size(Candidate *candidate)
{
  while (1)
  {
    printf("Digite o número de pessoas na família: ");

    if (scanf("%hu", &candidate->family_size) != 1)
    {
      display_error_message(ERROR_INVALID_INPUT);
      clear_input_buffer();
      candidate->family_size = 0;
      continue;
    }

    if (candidate->family_size <= 0)
    {
      display_error_message(ERROR_ZERO_OR_NEGATIVE);
      continue;
    }

    break;
  };
}

int get_family_scholarship_recipients(Candidate *candidate)
{
  while (1)
  {
    printf("Digite o número de familiares bolsistas na universidade: ");

    if (scanf("%hu", &candidate->family_scholarship_recipients) != 1)
    {
      display_error_message(ERROR_INVALID_INPUT);
      clear_input_buffer();
      candidate->family_scholarship_recipients = 0;
      continue;
    }

    if (candidate->family_scholarship_recipients < 0)
    {
      display_error_message(ERROR_NEGATIVE_VALUE);
      continue;
    }

    break;
  }
}

int get_has_disability(Candidate *candidate)
{
  while (1)
  {
    printf("Possui deficiência? (1-Sim, 0-Não): ");

    if (scanf("%d", (int *)&candidate->has_disability) != 1)
    {
      display_error_message(ERROR_INVALID_INPUT);
      clear_input_buffer();
      candidate->has_disability = false;
      continue;
    }

    if (candidate->has_disability != 0 && candidate->has_disability != 1)
    {
      display_error_message(ERROR_BINARY_EXPECTED);
      continue;
    }

    break;
  };
}

int get_high_school_average(Candidate *candidate)
{
  while (1)
  {
    printf("Digite a Média Geral do Ensino Médio (0 a 10): ");

    if (scanf("%f", &candidate->high_school_average) != 1)
    {
      display_error_message(ERROR_INVALID_INPUT);
      clear_input_buffer();
      candidate->high_school_average = 0.0f;
      continue;
    }

    if (candidate->high_school_average < 0 || candidate->high_school_average > 10)
    {
      display_error_message(ERROR_OUT_OF_RANGE);
      continue;
    }

    break;
  };
}

int get_internal_selection_score(Candidate *candidate)
{

  while (1)
  {
    printf("Digite a Nota do Processo Seletivo Interno (0 a 10): ");

    if (scanf("%f", &candidate->internal_selection_score) != 1)
    {
      display_error_message(ERROR_INVALID_INPUT);
      clear_input_buffer();
      candidate->internal_selection_score = 0.0f;
      continue;
    }

    if (candidate->internal_selection_score < 0 || candidate->internal_selection_score > 10)
    {
      display_error_message(ERROR_OUT_OF_RANGE);
      continue;
    }

    break;
  }
}

int get_is_public_school_student(Candidate *candidate)
{
  while (1)
  {
    printf("É aluno da rede pública? (1-Sim, 0-Não): ");

    if (scanf("%d", (int *)&candidate->is_public_school_student) != 1)
    {
      display_error_message(ERROR_INVALID_INPUT);
      clear_input_buffer();
      candidate->is_public_school_student = 0.0f;
      continue;
    }

    if (candidate->is_public_school_student != 0 && candidate->is_public_school_student != 1)
    {
      display_error_message(ERROR_BINARY_EXPECTED);
      continue;
    }

    break;
  }
}

int get_monthly_household_income(Candidate *candidate)
{
  while (1)
  {
    printf("Digite a renda mensal da família (R$): ");

    if (scanf("%f", &candidate->monthly_household_income) != 1)
    {
      display_error_message(ERROR_INVALID_INPUT);
      clear_input_buffer();
      candidate->monthly_household_income = false;
      continue;
    }

    if (candidate->monthly_household_income <= 0)
    {
      display_error_message(ERROR_ZERO_OR_NEGATIVE);
      continue;
    }

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
    printf("Motivo: %s\n", rejection_reason);
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

  switch (candidate.desired_scholarship_type)
  {
  case SCHOLARSHIP_FULL:
    if (candidate.per_capita_income > MAX_INCOME_FULL_SCHOLARSHIP)
    {
      print_application_result(&candidate, "Renda per capita superior a R$ 800.00.");
      break;
    }
    if (candidate.final_score < MIN_SCORE_FULL_SCHOLARSHIP)
    {
      print_application_result(&candidate, "Nota final inferior a 8.0.");
      break;
    }

    if (!candidate.is_public_school_student && !candidate.has_disability)
    {
      print_application_result(&candidate, "Não possui deficiência e não é da rede pública.");
      break;
    }

    if (candidate.family_scholarship_recipients > 0)
    {
      print_application_result(&candidate, "Possui familiares já bolsistas.");
      break;
    }

    print_application_result(&candidate, NULL);
    break;

  case SCHOLARSHIP_PARTIAL:
    if (candidate.per_capita_income > MAX_INCOME_PARTIAL_SCHOLARSHIP)
    {
      print_application_result(&candidate, "Renda per capita superior a R$ 1600.00.");
      break;
    }

    float min_score = MIN_SCORE_PARTIAL_SCHOLARSHIP;

    if (!candidate.is_public_school_student && !candidate.has_disability)
    {
      min_score = MIN_SCORE_PARTIAL_NO_PUBLIC_SCHOOL_NO_DISABILITY;
    }

    if (candidate.final_score < min_score)
    {
      char rejection_message[256];
      const char *message_template = (min_score == 7.5) ? "Nota final inferior a %.1f, exigido para a Bolsa Parcial. Candidato não é da rede pública e não possui deficiência." : "Nota final inferior a %.1f.";
      sprintf(rejection_message, message_template, min_score);
      print_application_result(&candidate, rejection_message);
      break;
    }

    if (candidate.family_scholarship_recipients > 1)
    {
      print_application_result(&candidate, "Número de familiares bolsistas superior ao permitido (máximo de 1 familiar para a Bolsa Parcial).");
      break;
    }

    print_application_result(&candidate, NULL);
    break;
  }
  return 0;
}
