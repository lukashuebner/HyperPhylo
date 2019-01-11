library(ggplot2)
library(dplyr)
library(readr)

data <- read_csv(
  file = "Documents/Uni/SS18/BioInfo Praktikum/results/rdda_vs_hybrid/boxplot_data.csv",
  col_types = cols(
    instance = col_character(),
    algorithm = col_character(),
    k = col_integer(),
    coreRCC = col_integer()
))

data %>%
  filter(coreRCC != 0) %>%
  ggplot(aes(x=instance, color=algorithm, y=coreRCC)) +
  geom_boxplot(position = "dodge") +
  #geom_jitter() +
  scale_y_log10() +
  theme_bw()

t.test(coreRCC ~ )