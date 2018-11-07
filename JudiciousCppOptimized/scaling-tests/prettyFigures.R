library(ggplot2)
library(dplyr)
library(readr)

testResults <- read_csv("testResults.csv")

# Strong scaling
strongResults <- testResults %>% filter(scaling == "strong")
series <- strongResults %>% filter(threads==1) %>% select(algorithm, scaling, machine, sites, k, runtime)
colnames(series)[colnames(series)=="runtime"] <- "baselineRuntime"
strongResults <- inner_join(strongResults, series)
strongResults$speedup <- strongResults$baselineRuntime / strongResults$runtime

strongResults %>%
  filter(algorithm=="aligned") %>% 
  ggplot(aes(x=threads, y=speedup, color=machine, shape=machine)) +
  geom_line() +
  geom_point() +
  geom_abline(slope=1, intercept=0) + 
  coord_fixed(ratio=1) +
  scale_x_continuous(limits = c(0, 32)) +
  scale_y_continuous(limits = c(0, 32)) +
  theme_light() +
  theme(panel.grid.minor = element_blank())

ggsave("strong-scaling.svg", width=7)

# Weak scaling

weakResults <- testResults %>% filter(scaling == "weak")
series <- weakResults %>% filter(threads==1) %>% select(algorithm, scaling, machine, k, runtime)
colnames(series)[colnames(series)=="runtime"] <- "baselineRuntime"
weakResults <- inner_join(weakResults, series)
weakResults$speedup <- weakResults$baselineRuntime / weakResults$runtime

weakResults %>%
  filter(algorithm=="aligned", k==160) %>% 
  ggplot(aes(x=threads, y=speedup, color=machine, shape=machine)) +
  geom_line() +
  geom_point() +
  geom_hline(yintercept=1) +
  scale_x_continuous(limits = c(0, 32)) +
  scale_y_continuous(limits = c(0, 2)) +
  theme_light() +
  theme(panel.grid.minor = element_blank())

ggsave("weak-scaling.svg", width=7)

