library(ggplot2)
library(dplyr)
library(readr)

testResults <- read_csv("testResults.csv")

# Strong scaling
strongResults <- testResults %>% filter(scaling == "strong")
singleThreadRuntime <- strongResults %>% filter(threads == 1) %>% select(runtime) %>% first()
strongResults$speedup <- singleThreadRuntime / strongResults$runtime

strongResults %>%
  ggplot(aes(x=threads, y=speedup, color=algorithm)) +
  geom_line() +
  geom_point() +
  scale_x_continuous(limits = c(0, 32), breaks = c(1, 2, 4, 8, 16, 32)) +
  scale_y_continuous(limits = c(0, 32), breaks = round(strongResults$speedup, digits = 1)) +
  theme_light() +
  theme(panel.grid.minor = element_blank())

# Weak scaling

weakResults <- testResults %>% filter(scaling == "weak")
singleThreadRuntime <- weakResults %>% filter(threads == 1) %>% select(runtime) %>% first()
weakResults$speedup <- singleThreadRuntime / weakResults$runtime

weakResults %>%
  ggplot(aes(x=threads, y=speedup, color=algorithm)) +
  geom_line() +
  geom_point() +
  scale_x_continuous(limits = c(0, 16), breaks = c(1, 2, 4, 8, 16, 32)) +
  scale_y_continuous(limits = c(0, 1), breaks = round(weakResults$speedup, digits = 2)) +
  theme_light() +
  theme(panel.grid.minor = element_blank())
