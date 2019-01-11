library(ggplot2)
library(dplyr)
library(readr)

data <- read_csv("Documents/Uni/SS18/BioInfo Praktikum/results/rdda_vs_hybrid/boxplot_data.csv")

data %>%
  ggplot(aes(x = c(algorithm, instance), color = algorithm, y = coreRCC)) +
  geom_boxplot()

