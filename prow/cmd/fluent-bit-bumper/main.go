/*
Copyright 2019 The Kubernetes Authors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

package main

import (
	"fmt"
	"github.com/sirupsen/logrus"
	flag "github.com/spf13/pflag"
	"io/ioutil"

	"k8s.io/test-infra/prow/cmd/generic-autobumper/bumper"

	"sigs.k8s.io/yaml"
)

var _ bumper.PRHandler = (*client)(nil)

type client struct {
	images   map[string]string
	versions map[string][]string
}

// Changes returns a slice of functions, each one does some stuff, and
// returns commit message for the changes
func (c *client) Changes() []func() (string, error) {
	return []func() (string, error){
		func() (string, error) {
			return "Bumping", nil
		},
	}
}

// PRTitleBody returns the body of the PR, this function runs after each commit
func (c *client) PRTitleBody() (string, string, error) {
	return "[Test PR] Bumped", "Bumped" + "\n", nil
}

func parseOptions() (*bumper.Options, error) {
	var config string

	flag.StringVar(&config, "config", "", "The path to the config file for the fluent bit bumper.")
	flag.Parse()

	var opts bumper.Options
	data, err := ioutil.ReadFile(config)
	if err != nil {
		return nil, fmt.Errorf("read %q: %w", config, err)
	}

	if err := yaml.Unmarshal(data, &opts); err != nil {
		return nil, fmt.Errorf("unmarshal %q: %w", config, err)
	}

	return &opts, nil
}

func main() {
	logrus.SetLevel(logrus.DebugLevel)
	opts, err := parseOptions()
	if err != nil {
		logrus.WithError(err).Fatalf("Failed to run the bumper tool")
	}

	if err := bumper.Run(opts, &client{}); err != nil {
		logrus.WithError(err).Fatalf("Failed to run the bumper tool")
	}
}