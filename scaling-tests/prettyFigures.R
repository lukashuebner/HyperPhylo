library(ggplot2)
library(dplyr)
library(readr)

testResults <- read_csv("testResults.csv", col_names = TRUE,
  cols(
    algorithm = col_character(),
    pinning = col_character(),
    scaling = col_character(),
    machine = col_character(),
    sites = col_integer(),
    k = col_integer(),
    threads = col_integer()
))

# Strong scaling
strongResults <- testResults %>% filter(scaling == "strong")
series <- strongResults %>% filter(threads==1) %>% select(algorithm, pinning, scaling, machine, sites, k, runtime)
colnames(series)[colnames(series)=="runtime"] <- "baselineRuntime"
strongResults <- inner_join(strongResults, series)
strongResults$speedup <- strongResults$baselineRuntime / strongResults$runtime

strongResults %>%
  filter(k==50, sites==50000, pinning=="balanced") %>% 
  ggplot(aes(x=threads, y=speedup, color=machine, shape=machine,
             group=interaction(algorithm, pinning, machine),
             linetype=algorithm)) +
  geom_line() +
  geom_point() +
  geom_abline(slope=1, intercept=0) + 
  coord_fixed(ratio=1) +
  scale_x_continuous(limits = c(0, 32), breaks = unique(strongResults$threads)) +
  scale_y_continuous(limits = c(0, 32), breaks = seq(from = 0, to = 32, by = 2)) +
  theme_light() +
  theme(panel.grid.minor = element_blank())

ggsave("strong-scaling.svg", width=7)

# Weak scaling

weakResults <- testResults %>% filter(scaling == "weak")
series <- weakResults %>% filter(threads==1) %>% select(algorithm, pinning, scaling, machine, k, runtime)
colnames(series)[colnames(series)=="runtime"] <- "baselineRuntime"
weakResults <- inner_join(weakResults, series)

weakResults %>%
  filter(algorithm=="aligned", k==160) %>%
  ggplot(aes(x=threads, y=runtime/1000/(sites/1000), color=machine, shape=algorithm,
      group=interaction(algorithm, pinning, machine),
      linetype=pinning)) + 
  geom_line() +
  geom_point() +
  scale_x_continuous(limits = c(0, 32), breaks=unique(weakResults$threads)) +
  scale_y_continuous(name = "runtime per 1,000 sites [s]") +
  theme_light() +
  theme(panel.grid.minor = element_blank())

ggsave("weak-scaling.svg", width=7)

# Absolute running times
strongResults <- testResults %>% filter(scaling == "strong", k==50, sites==50000)
strongResults %>%
  filter(k==50, machine=="i10pc127") %>%
  ggplot(aes(x=threads, y=runtime/(1000*3600), color=algorithm, shape=algorithm,
             group=interaction(algorithm, pinning, machine),
             linetype=pinning)) +
  geom_line() +
  geom_point() +
  scale_x_continuous(limits = c(0, 32), breaks=unique(weakResults$threads)) +
  scale_y_continuous(name = "runtime [h]") +
  theme_light() +
  theme(panel.grid.minor = element_blank())

ggsave("runtimes.svg", width=7)
